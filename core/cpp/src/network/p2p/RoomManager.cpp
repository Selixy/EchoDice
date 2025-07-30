#include "network/p2p/RoomManager.hpp"
#include "Logger.hpp"
#include <nlohmann/json.hpp>
#include <random>

using json = nlohmann::json;

namespace p2p {

static constexpr int CODE_LEN = 8;

RoomManager::RoomManager(const std::string& localId)
  : localId_(localId),
    signaler_("0.peerjs.com", 443, "peerjs", localId, "auto")
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
        LOG_INFO("Room créée, code = " + roomCode_);

        // Callback à l’ouverture WebSocket
        signaler_.setOnOpen([this]() {
            LOG_INFO("WebSocket ouverte");
            LOG_INFO("Enregistrement dans la room " + roomCode_);
            json j;
            j["room"] = roomCode_;
            j["id"]   = localId_;
            signaler_.send("ROOM_JOIN", "", j.dump());
        });

        // Callback quand un pair rejoint ou message arrivé
        signaler_.setOnMessage([this](auto&& type, auto&& src, auto&& payload) {
            if (type == "ROOM_JOIN" && src != localId_) {
                if (peerJoinedCb_) peerJoinedCb_(src);
            } else if (type == "MESSAGE") {
                if (messageCb_) {
                    auto j = json::parse(payload);
                    messageCb_(src, j.value("message", std::string{}));
                }
            }
        });

        signaler_.connect();
    }
    return roomCode_;
}

void RoomManager::join(const std::string& code) {
    roomCode_ = code;
    signaler_.setOnOpen([this]() {
        LOG_INFO("WebSocket ouverte");
        LOG_INFO("Enregistrement dans la room " + roomCode_);
        json j;
        j["room"] = roomCode_;
        j["id"]   = localId_;
        signaler_.send("ROOM_JOIN", "", j.dump());
    });
    signaler_.connect();
}

void RoomManager::sendMessage(const std::string& peerId,
                              const std::string& message) {
    json j;
    j["message"] = message;
    signaler_.send("MESSAGE", peerId, j.dump());
}

void RoomManager::onPeerJoined(PeerCallback cb) {
    peerJoinedCb_ = std::move(cb);
}

void RoomManager::onMessageReceived(MessageCallback cb) {
    messageCb_ = std::move(cb);
}

void RoomManager::shutdown() {
    signaler_.shutdown();
    roomCode_.clear();
}

} // namespace p2p
