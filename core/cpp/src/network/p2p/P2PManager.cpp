// P2PManager.cpp
#include "network/p2p/P2PManager.hpp"
#include "network/serveur/PeerJSWebSocket.hpp"
#include "Logger.hpp"
#include <nlohmann/json.hpp>
#include <random>

using json = nlohmann::json;

namespace p2p {

static std::unique_ptr<PeerJS::PeerJSWebSocket> signal;
static std::string localId;
static std::string roomCode;
static PeerCallback    peerJoinedCb;
static MessageCallback messageCb;

// Génère un code de room aléatoire (8 caractères alphanumériques)
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

    // Callback des messages de signalisation
    signal->setOnMessage([](const std::string& type,
                            const std::string& src,
                            const std::string& payload)
    {
        if (type == "ROOM_JOIN") {
            // Un peer vient de rejoindre la room
            if (src != localId && peerJoinedCb) {
                LOG_INFO("Peer rejoint : " + src);
                peerJoinedCb(src);
            }
        }
        else if (type == "MESSAGE") {
            // Message P2P (ou simulé) reçu
            auto j = json::parse(payload);
            std::string msg = j.value("message", std::string{});
            LOG_INFO("Message reçu de " + src + " : " + msg);
            if (messageCb) messageCb(src, msg);
        }
    });
}

std::string getCode() {
    if (roomCode.empty()) {
        roomCode = generateRoomCode();
        LOG_INFO("Code de room généré : " + roomCode);
    }
    return roomCode;
}

void connectTo(const std::string& code) {
    roomCode = code;
    if (!signal) {
        // Si initLocal n'a pas encore été appelé
        initLocal(localId);
    }
    // À l'ouverture du WebSocket, on annonce notre arrivée dans la room
    signal->setOnOpen([&](){
        json j;
        j["room"] = roomCode;
        j["id"]   = localId;
        signal->send("ROOM_JOIN", /* dst = */ "", j.dump());
        LOG_INFO("Connexion établie, enregistrement dans la room " + roomCode);
    });
    signal->connect();
}

void sendMessage(const std::string& peerId,
                 const std::string& message)
{
    if (!signal) {
        LOG_ERROR("Impossible d'envoyer le message : signalisation non initialisée");
        return;
    }
    json j;
    j["message"] = message;
    signal->send("MESSAGE", peerId, j.dump());
    LOG_INFO("Message envoyé à " + peerId + " : " + message);
}

void shutdown() {
    LOG_INFO("Arrêt de la signalisation P2P");
    signal.reset();
}

void onPeerJoined(PeerCallback cb) {
    peerJoinedCb = std::move(cb);
}

void onMessageReceived(MessageCallback cb) {
    messageCb = std::move(cb);
}

} // namespace p2p
