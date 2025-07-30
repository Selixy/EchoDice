// core/cpp/src/network/p2p/P2PManager.cpp

#include "network/p2p/P2PManager.hpp"
#include "network/serveur/PeerJSWebSocket.hpp"

#include <nlohmann/json.hpp>
#include <random>
#include <iostream>

using json = nlohmann::json;

namespace p2p {

static std::unique_ptr<PeerJS::PeerJSWebSocket> signal;
static std::string localId;
static std::string roomCode;

/// Génère un code de room aléatoire (8 caractères alphanumériques).
static std::string generateRoomCode() {
  static const char alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  static std::mt19937_64 rng{std::random_device{}()};
  std::uniform_int_distribution<> dist(0, sizeof(alpha) - 2);
  constexpr int LEN = 8;
  std::string code;
  code.reserve(LEN);
  for (int i = 0; i < LEN; ++i) {
    code += alpha[dist(rng)];
  }
  return code;
}

/// Instancie et configure une seule fois le WebSocket de signalling.
static void ensureSignal() {
  if (signal) return;

  signal = std::make_unique<PeerJS::PeerJSWebSocket>(
    /*host=*/  "0.peerjs.com",
    /*port=*/  443,
    /*key=*/   "peerjs",
    /*id=*/    localId,
    /*token=*/ "auto"
  );

  // Callback pour tous les messages de signalling
  signal->setOnMessage([](const std::string& type,
                          const std::string& src,
                          const std::string& payload)
  {
    if (type == "ROOM_JOIN") {
      if (src != localId) {
        std::cout << "[INFO] Le pair \"" << src
                  << "\" a rejoint la room \"" << roomCode << "\"\n";
      }
    }
    else if (type == "MESSAGE") {
      auto j = json::parse(payload);
      auto m = j.value("message", std::string{});
      std::cout << "[INFO] Message reçu de \"" << src
                << "\": " << m << "\n";
    }
    else {
      std::cout << "[WARN] Signal inconnu de type \"" << type << "\"\n";
    }
  });
}

void initLocal(const std::string& id) {
  localId = id;
  roomCode.clear();
  ensureSignal();
  std::cout << "[INFO] ID local initialisé sur \"" << localId << "\"\n";
}

std::string getCode() {
  if (roomCode.empty()) {
    roomCode = generateRoomCode();
    std::cout << "[INFO] Code de room généré : " << roomCode << "\n";
  }
  return roomCode;
}

void connectTo(const std::string& code) {
  roomCode = code;
  ensureSignal();

  // À l’ouverture, on s’enregistre dans la room
  signal->setOnOpen([code](){
    std::cout << "[INFO] WebSocket ouvert, enregistrement dans la room \""
              << code << "\"\n";
    json j;
    j["room"] = code;
    j["id"]   = localId;
    signal->send("ROOM_JOIN", /*dst=*/"", j.dump());
  });

  std::cout << "[INFO] Connexion au serveur PeerJS pour room \""
            << code << "\"\n";
  signal->connect();
}

void sendMessage(const std::string& peerId,
                 const std::string& message)
{
  ensureSignal();
  // Envoi direct — si la socket n'est pas (encore) ouverte, le
  // WebSocket interne gérera ou rejettera l'envoi.
  signal->send("MESSAGE", peerId, json{{"message", message}}.dump());
  std::cout << "[INFO] Message envoyé à \"" << peerId
            << "\": " << message << "\n";
}

void shutdown() {
  if (signal) {
    std::cout << "[INFO] Arrêt du sous-système P2P\n";
    signal.reset();
  } else {
    std::cout << "[WARN] shutdown appelé sans initialisation\n";
  }
}

} // namespace p2p
