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
    code.reserve(8);
    for (int i = 0; i < 8; ++i) {
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

        // 1. Callback ouverture WS
        signaler_->setOnOpen([this]() {
            LOG_INFO("WebSocket PeerJS ouverte pour " + localId_);
            // 2. Envoie le message ROOM_JOIN à tous (broadcast)
            json j;
            j["room"] = roomCode_;
            j["id"]   = localId_;
            signaler_->send("ROOM_JOIN", "", j.dump());
        });

        // 3. Callback message signaler
        signaler_->setOnMessage([this](auto&& type, auto&& src, auto&& payload) {
            handleSignalingMessage(type, src, payload);
        });

        signaler_->connect();
    }
    return roomCode_;
}

void RoomManager::join(const std::string& code) {
    roomCode_ = code;

    signaler_ = std::make_unique<Signaler>(
        "0.peerjs.com", 443, "peerjs", localId_, "auto"
    );

    // Callback ouverture WS
    signaler_->setOnOpen([this]() {
        LOG_INFO("WebSocket PeerJS ouverte pour " + localId_);
        // Envoie JOIN pour signaler sa présence dans la room
        json j;
        j["room"] = roomCode_;
        j["id"]   = localId_;
        signaler_->send("ROOM_JOIN", "", j.dump());
    });

    // Callback messages des autres
    signaler_->setOnMessage([this](auto&& type, auto&& src, auto&& payload) {
        handleSignalingMessage(type, src, payload);
    });

    signaler_->connect();
}

void RoomManager::handleSignalingMessage(const std::string& type,
                                         const std::string& src,
                                         const std::string& payload)
{
    // Quand on reçoit un ROOM_JOIN d'un autre peer
    if (type == "ROOM_JOIN" && src != localId_) {
        LOG_INFO("Un peer a rejoint la room: " + src);
        if (peerJoinedCb_) peerJoinedCb_(src);
    }
    // Plus tard : autres types (MESSAGE, OFFER…)
}

void RoomManager::onPeerJoined(PeerJoinedCallback cb) {
    peerJoinedCb_ = std::move(cb);
}

void RoomManager::shutdown() {
    if (signaler_) signaler_->shutdown();
    roomCode_.clear();
}
