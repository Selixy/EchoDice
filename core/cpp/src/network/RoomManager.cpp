// core/cpp/src/network/RoomManager.cpp

#include "network/RoomManager.hpp"
#include "Logger.hpp"
#include <nlohmann/json.hpp>
#include <random>

using json = nlohmann::json;

static constexpr int CODE_LEN = 8;

RoomManager::RoomManager(const std::string& localId)
  : localId_(localId)
{
    setupPeerManager();
}

void RoomManager::setupPeerManager() {
    peerMgr_ = std::make_unique<PeerManager>(
        [this](const std::string& type,
               const std::string& dst,;   
               const std::string& payload)
        {
            if (signaler_) {
                signaler_->send(type, dst, payload);
            }
        }
    );

    // Ex. pour brancher tes DataChannel :
    // peerMgr_->onDataChannelOpen(...);
    // peerMgr_->onDataChannelMessage(...);
}

std::string RoomManager::generateRoomCode() {
    static const char alpha[] =
        "ABCDEFGHIJKLMNPQRSTUVWXYZ"
        "0123456789"
        "abcdefghijklmnpqrstuvwxyz"
        "0123456789";
    static std::mt19937_64 rng{ std::random_device{}() };
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

        // On n'envoie plus de ROOM_JOIN en REST/unicast ici
        signaler_->setOnOpen([this]() {
            LOG_INFO("WebSocket ouverte pour " + localId_);
        });

        signaler_->setOnMessage([this](auto&& type,
                                       auto&& src,
                                       auto&& payload) {
            LOG_DEBUG("RECU: type=" + type + " src=" + src);
            handleSignalingMessage(type, src, payload);
        });

        signaler_->connect();
    }
    return roomCode_;
}

void RoomManager::join(const std::string& code) {
    roomCode_ = code;
    LOG_INFO("Join room, code = " + roomCode_);

    signaler_ = std::make_unique<Signaler>(
        "0.peerjs.com", 443, "peerjs", localId_, "auto"
    );

    signaler_->setOnOpen([this]() {
        LOG_INFO("WebSocket ouverte pour " + localId_);
    });

    signaler_->setOnMessage([this](auto&& type,
                                   auto&& src,
                                   auto&& payload) {
        LOG_DEBUG("RECU: type=" + type + " src=" + src);
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
    auto data = json::parse(payload);

    if (type == "ROOM_JOIN") {
        // Si tu veux toujours exploiter ROOM_JOIN pour déclencher
        LOG_INFO("ROOM_JOIN recu de " + src);
        peerMgr_->connectToPeer(src);
        if (peerJoinedCb_) peerJoinedCb_(src);
    }
    else if (type == "SDP_OFFER") {
        peerMgr_->handleOffer(src, data["sdp"].get<std::string>());
    }
    else if (type == "SDP_ANSWER") {
        peerMgr_->handleAnswer(src, data["sdp"].get<std::string>());
    }
    else if (type == "ICE_CANDIDATE") {
        auto cand = data["candidate"].get<std::string>();
        auto mid  = data["mid"].get<std::string>();
        peerMgr_->handleIce(src, cand, mid);
    }
}

void RoomManager::shutdown() {
    if (signaler_) signaler_->shutdown();
    roomCode_.clear();
}
