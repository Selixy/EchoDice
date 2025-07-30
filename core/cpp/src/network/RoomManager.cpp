#include "network/RoomManager.hpp"
#include "Logger.hpp"
#include <nlohmann/json.hpp>
#include <random>

using json = nlohmann::json;
static constexpr int CODE_LEN = 8;

RoomManager::RoomManager(const std::string& localId)
    : localId_(localId)
{}

std::string RoomManager::generateRoomCode() {
    static const char alpha[] =
        "ABCDEFGHIJKLMNPQRSTUVWXYZ"
        "0123456789"
        "abcdefghijklmnpqrstuvwxyz"
        "0123456789";
    static std::mt19937_64 rng{std::random_device{}()};
    std::uniform_int_distribution<> dist(0, sizeof(alpha) - 2);
    std::string code;
    code.reserve(CODE_LEN);
    for (int i = 0; i < CODE_LEN; ++i) {
        code += alpha[dist(rng)];
    }
    return code;
}

const std::string& RoomManager::createAndJoin() {
    if (roomCode_.empty()) {
        roomCode_ = generateRoomCode();
        LOG_INFO("Room creee, code = " + roomCode_);

        signaler_ = std::make_unique<Signaler>(
            "0.peerjs.com", 443, "peerjs", localId_, "auto"
        );

        signaler_->setOnOpen([this]() {
            LOG_INFO("WebSocket PeerJS ouverte pour " + localId_);
            json j;
            j["room"] = roomCode_;
            j["id"]   = localId_;
            signaler_->send("ROOM_JOIN", "", j.dump());
        });

        signaler_->setOnMessage([this](auto&& type, auto&& src, auto&& payload) {
            LOG_DEBUG("RECU: type=" + type + " src=" + src + " payload=" + payload);
            handleSignalingMessage(type, src, payload);
        });

        signaler_->connect();
    }
    return roomCode_;
}

void RoomManager::join(const std::string& code) {
    roomCode_ = code;

    LOG_INFO("Connexion a PeerJS WS : 0.peerjs.com:443");
    signaler_ = std::make_unique<Signaler>(
        "0.peerjs.com", 443, "peerjs", localId_, "auto"
    );

    signaler_->setOnOpen([this]() {
        LOG_INFO("WebSocket PeerJS ouverte pour " + localId_);
        json j;
        j["room"] = roomCode_;
        j["id"]   = localId_;
        signaler_->send("ROOM_JOIN", "", j.dump());
    });

    signaler_->setOnMessage([this](auto&& type, auto&& src, auto&& payload) {
        LOG_DEBUG("RECU: type=" + type + " src=" + src + " payload=" + payload);
        handleSignalingMessage(type, src, payload);
    });

    signaler_->connect();
}

const std::string& RoomManager::getRoomCode() const {
    return roomCode_;
}

void RoomManager::onPeerJoined(PeerJoinedCallback cb) {
    peerJoinedCb_ = std::move(cb);
}

void RoomManager::handleSignalingMessage(const std::string& type,
                                         const std::string& src,
                                         const std::string& payload) {
    if (type == "ROOM_JOIN") {
        LOG_INFO("ROOM_JOIN recu de " + src);
        if (src != localId_ && peerJoinedCb_) {
            peerJoinedCb_(src);
        }
    }
}

void RoomManager::shutdown() {
    if (signaler_) signaler_->shutdown();
    roomCode_.clear();
}
