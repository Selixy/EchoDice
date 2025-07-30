// P2PManager.cpp
#include "network/p2p/P2PManager.hpp"
#include "network/serveur/PeerJSWebSocket.hpp"
#include <nlohmann/json.hpp>
#include <random>
#include <iostream>

using json = nlohmann::json;

namespace p2p {

// Unique instance de la connexion de signaling
static std::unique_ptr<PeerJS::PeerJSWebSocket> signal;

// Notre propre ID et le code de la room
static std::string localId;
static std::string roomCode;

// Callbacks enregistrés par l’API
static PeerCallback    peerJoinedCb   = nullptr;
static MessageCallback messageCb      = nullptr;

/// Génère un code de room aléatoire (8 caractères alphanumériques).
static std::string generateRoomCode() {
    static const char alpha[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
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

/// Initialise la connexion WebSocket et les handlers de signaling.
void initLocal(const std::string& id) {
    localId = id;
    signal = std::make_unique<PeerJS::PeerJSWebSocket>(
        "0.peerjs.com", 443, "peerjs", localId, "auto"
    );

    signal->setOnOpen([&](){
        std::cout << "[DEBUG] Signaling WebSocket ouvert pour ID=" 
                  << localId << std::endl;
    });

    signal->setOnMessage(
      [](const std::string& type,
         const std::string& src,
         const std::string& payload)
    {
        if (type == "ROOM_JOIN") {
            // Un nouveau peer vient de rejoindre la même room
            std::cout << "[INFO] Peer " << src 
                      << " a rejoint la room." << std::endl;
            if (peerJoinedCb && src != localId) {
                peerJoinedCb(src);
            }
        }
        else if (type == "MESSAGE") {
            // Message P2P reçu via signaling
            auto j = json::parse(payload);
            std::string msg = j.value("message", std::string{});
            std::cout << "[INFO] Message reçu de " 
                      << src << " : " << msg << std::endl;
            if (messageCb) {
                messageCb(src, msg);
            }
        }
    });
}

/// Retourne (et génère au besoin) le code de room.
std::string getCode() {
    if (roomCode.empty()) {
        roomCode = generateRoomCode();
    }
    return roomCode;
}

/// Se connecte à la room identifiée par `code`.
void connectTo(const std::string& code) {
    roomCode = code;
    if (!signal) {
        initLocal(localId);
    }
    signal->setOnOpen([&](){
        json j = {
          {"room", roomCode},
          {"id",   localId}
        };
        signal->send("ROOM_JOIN", /*dst=*/"", j.dump());
        std::cout << "[INFO] Enregistrement dans la room " 
                  << roomCode << std::endl;
    });
    signal->connect();
}

/// Envoie un message direct à `peerId` via signaling.
void sendMessage(const std::string& peerId,
                 const std::string& message)
{
    if (!signal) {
        std::cerr << "[WARN] Signaling non initialisé !" << std::endl;
        return;
    }
    json j = { {"message", message} };
    signal->send("MESSAGE", peerId, j.dump());
    std::cout << "[INFO] Envoi de message à " << peerId 
              << " : " << message << std::endl;
}

/// Ferme proprement la connexion.
void shutdown() {
    if (signal) {
        signal.reset();
        std::cout << "[DEBUG] Signaling WebSocket fermé." << std::endl;
    }
}

/// Enregistre un callback pour l’arrivée d’un nouveau peer.
void onPeerJoined(PeerCallback cb) {
    peerJoinedCb = std::move(cb);
}

/// Enregistre un callback pour la réception de messages.
void onMessageReceived(MessageCallback cb) {
    messageCb = std::move(cb);
}

} // namespace p2p
