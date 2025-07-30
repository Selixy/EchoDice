// core/cpp/src/network/p2p/P2PManager.cpp

#include "network/p2p/P2PManager.hpp"
#include "network/serveur/PeerJSWebSocket.hpp"
#include <nlohmann/json.hpp>
#include <random>
#include <iostream>
#include <vector>

using json = nlohmann::json;

namespace p2p {

static std::unique_ptr<PeerJS::PeerJSWebSocket> signal;
static std::string localId;
static std::string roomCode;
static bool wsOpen = false;
static std::vector<std::pair<std::string, std::string>> pendingMessages;
static PeerCallback peerJoinedCb;
static MessageCallback messageCb;

/// Génère un code aléatoire de 8 caractères.
static std::string generateRoomCode() {
    static const char alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static std::mt19937_64 rng{std::random_device{}()};
    std::uniform_int_distribution<> dist(0, sizeof(alpha) - 2);
    std::string code;
    code.reserve(8);
    for (int i = 0; i < 8; ++i) code += alpha[dist(rng)];
    return code;
}

void initLocal(const std::string& id) {
    localId = id;
    signal = std::make_unique<PeerJS::PeerJSWebSocket>(
        "0.peerjs.com", 443, "peerjs", localId, "auto"
    );

    // Quand la WS s'ouvre…
    signal->setOnOpen([&]() {
        wsOpen = true;
        std::cout << "[INFO] WebSocket ouverte\n";
        // Rejoindre la room si déjà défini
        if (!roomCode.empty()) {
            json j; j["room"] = roomCode; j["id"] = localId;
            signal->send("ROOM_JOIN", "", j.dump());
            std::cout << "[INFO] Enregistrement dans la room " << roomCode << "\n";
        }
        // Vider le tampon
        for (auto& [peer,msg] : pendingMessages) {
            json m; m["message"] = msg;
            signal->send("MESSAGE", peer, m.dump());
            std::cout << "[INFO] Envoi différé vers " << peer
                      << " : " << msg << "\n";
        }
        pendingMessages.clear();
    });

    // Message signalisation entrant
    signal->setOnMessage([](const std::string& type,
                            const std::string& src,
                            const std::string& payload)
    {
        if (type == "ROOM_JOIN" && src != localId && peerJoinedCb) {
            std::cout << "[INFO] Peer rejoint : " << src << "\n";
            peerJoinedCb(src);
        }
        else if (type == "MESSAGE" && messageCb) {
            auto j = json::parse(payload);
            auto txt = j.value("message", std::string{});
            std::cout << "[INFO] Message reçu de " << src
                      << " : " << txt << "\n";
            messageCb(src, txt);
        }
    });
}

std::string getCode() {
    if (roomCode.empty())
        roomCode = generateRoomCode();
    return roomCode;
}

void connectTo(const std::string& code) {
    roomCode = code;
    if (!signal) initLocal(localId);
    signal->connect();
}

void sendMessage(const std::string& peerId,
                 const std::string& message)
{
    if (!wsOpen) {
        std::cout << "[WARN] WS pas ouverte, bufferisation du message\n";
        pendingMessages.emplace_back(peerId, message);
        return;
    }
    json j; j["message"] = message;
    signal->send("MESSAGE", peerId, j.dump());
    std::cout << "[INFO] Envoi de message à " << peerId
              << " : " << message << "\n";
}

void shutdown() {
    if (signal) signal.reset();
    wsOpen = false;
    pendingMessages.clear();
    std::cout << "[INFO] Sous-système P2P arrêté\n";
}

void onPeerJoined(PeerCallback cb) {
    peerJoinedCb = std::move(cb);
}

void onMessageReceived(MessageCallback cb) {
    messageCb = std::move(cb);
}

} // namespace p2p
