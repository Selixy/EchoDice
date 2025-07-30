// P2PManager.cpp

#include "Logger.hpp"
#include "network/p2p/P2PManager.hpp"
#include "network/serveur/PeerJSWebSocket.hpp"
#include <nlohmann/json.hpp>
#include <random>

using json = nlohmann::json;

namespace p2p {

// Unique pointer vers le WebSocket de signalisation
static std::unique_ptr<PeerJS::PeerJSWebSocket> signal;
static std::string localId;
static std::string roomCode;

static PeerCallback    peerJoinedCb;
static MessageCallback messageCb;

// Génère un code de room aléatoire (8 caractères alphanumériques).
static std::string generateRoomCode() {
    static const char alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
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

void initLocal(const std::string& id) {
    localId = id;
    signal = std::make_unique<PeerJS::PeerJSWebSocket>(
        "0.peerjs.com", 443, "peerjs", localId, "auto"
    );

    // Dès qu’on reçoit un message signalling…
    signal->setOnMessage([](const std::string& type,
                            const std::string& src,
                            const std::string& payload)
    {
        if (type == "ROOM_JOIN") {
            // un nouveau peer vient de rejoindre la room
            if (src != localId && peerJoinedCb) {
                LOG_INFO("Peer rejoint la room : " + src);
                peerJoinedCb(src);
            }
        }
        else if (type == "MESSAGE") {
            // message P2P d’un autre peer
            auto j = json::parse(payload);
            std::string msg = j.value("message", std::string{});
            // **Log côté réception**
            LOG_INFO("Message reçu de " + src + " : " + msg);
            if (messageCb) {
                messageCb(src, msg);
            }
        }
    });
}

std::string getCode() {
    if (roomCode.empty()) {
        roomCode = generateRoomCode();
    }
    return roomCode;
}

void connectTo(const std::string& code) {
    roomCode = code;
    if (!signal) {
        initLocal(localId);
    }
    signal->setOnOpen([&](){
        // à l’ouverture du websocket, on s’enregistre dans la room
        json j;
        j["room"] = roomCode;
        j["id"]   = localId;
        LOG_INFO("Connexion établie, enregistrement dans la room " + roomCode);
        signal->send("ROOM_JOIN", /*dst=*/"", j.dump());
    });
    signal->connect();
}

void sendMessage(const std::string& peerId,
                 const std::string& message)
{
    json j;
    j["message"] = message;
    LOG_INFO("Envoi de message à " + peerId + " : " + message);
    signal->send("MESSAGE", peerId, j.dump());
}

void shutdown() {
    LOG_INFO("Arrêt du sous-système P2P");
    signal.reset();
}

void onPeerJoined(PeerCallback cb) {
    peerJoinedCb = std::move(cb);
}

void onMessageReceived(MessageCallback cb) {
    messageCb = std::move(cb);
}

} // namespace p2p
