// src/network/p2p/P2PManager.cpp

#include "network/p2p/P2PManager.hpp"
#include "network/serveur/PeerJSWebSocket.hpp"
#include "Logger.hpp"               // pour LOG_INFO, LOG_WARN…
#include <nlohmann/json.hpp>
#include <random>

using json = nlohmann::json;

namespace p2p {

static std::unique_ptr<PeerJS::PeerJSWebSocket> signal;
static std::string localId;
static std::string roomCode;

static PeerCallback    peerJoinedCb;
static MessageCallback messageCb;

/// Génère un code de room aléatoire (8 caractères alphanumériques).
static std::string generateRoomCode() {
    static const char alpha[] = "ABCDEFGHIJKLMNPQRSTUVWXYZ0123456789abcdefghijklmnpqrstuvwxyz0123456789";
    static std::mt19937_64 rng{std::random_device{}()};
    std::uniform_int_distribution<> dist(0, sizeof(alpha) - 2);
    constexpr int CODE_LEN = 8;
    std::string code;
    code.reserve(CODE_LEN);
    for (int i = 0; i < CODE_LEN; ++i) {
        code += alpha[dist(rng)];
    }
    return code;
}

/// Initialise uniquement le WebSocket et les handlers de messages.
void initLocal(const std::string& id) {
    localId = id;
    signal = std::make_unique<PeerJS::PeerJSWebSocket>(
        "0.peerjs.com", 443, "peerjs", localId, "auto"
    );

    signal->setOnMessage([](const std::string& type,
                            const std::string& src,
                            const std::string& payload)
    {
        if (type == "ROOM_JOIN") {
            if (src != localId && peerJoinedCb) {
                peerJoinedCb(src);
            }
        }
        else if (type == "MESSAGE") {
            if (messageCb) {
                auto j = json::parse(payload);
                messageCb(src, j.value("message", std::string{}));
            }
        }
    });
}

/// Retourne le code de room *ET* s’y inscrit immédiatement.
std::string getCode() {
    if (roomCode.empty()) {
        roomCode = generateRoomCode();
        LOG_INFO("Room créée, code = " + roomCode);

        if (!signal) {
            initLocal(localId);
        }

        // Dès l’ouverture de la socket, on s’y enregistre
        signal->setOnOpen([&]() {
            LOG_INFO("WebSocket ouverte");
            LOG_INFO("Enregistrement dans la room " + roomCode);
            json j;
            j["room"] = roomCode;
            j["id"]   = localId;
            signal->send("ROOM_JOIN", /*dst=*/"", j.dump());
        });

        signal->connect();
    }

    return roomCode;
}

/// Se connecte explicitement à une room existante (utile si tu veux
/// rejoindre après coup une room dont tu as déjà le code).
void connectTo(const std::string& code) {
    roomCode = code;
    if (!signal) {
        initLocal(localId);
    }
    signal->setOnOpen([&]() {
        LOG_INFO("WebSocket ouverte");
        LOG_INFO("Enregistrement dans la room " + roomCode);
        json j;
        j["room"] = roomCode;
        j["id"]   = localId;
        signal->send("ROOM_JOIN", "", j.dump());
    });
    signal->connect();
}

void sendMessage(const std::string& peerId,
                 const std::string& message)
{
    json j;
    j["message"] = message;
    signal->send("MESSAGE", peerId, j.dump());
}

void shutdown() {
    signal.reset();
}

void onPeerJoined(PeerCallback cb) {
    peerJoinedCb = std::move(cb);
}

void onMessageReceived(MessageCallback cb) {
    messageCb = std::move(cb);
}

} // namespace p2p
