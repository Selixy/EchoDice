#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <rtc/peerconnection.hpp>
#include "network/serveur/PeerJSWebSocket.hpp"

namespace p2p {

using PeerCallback    = std::function<void(const std::string& peerId)>;
using MessageCallback = std::function<void(const std::string& peerId, const std::string& msg)>;

/// Initialise le signaling (à appeler une seule fois, après Set_ID).
void initLocal(const std::string& id);

/// Génère ou retourne le code de room.
std::string getCode();

/// Se connecte à la room existante (ouvre le WebSocket et envoie ROOM_JOIN).
void connectTo(const std::string& roomCode);

/// Envoie un message de données au peer donné.
void sendMessage(const std::string& peerId, const std::string& message);

/// Ferme toutes les connexions.
void shutdown();

/// Callback lorsqu’un pair entre dans la room (avant négociation).
void onPeerJoined(PeerCallback cb);

/// Callback lorsqu’un pair quitte la room.
void onPeerLeft(PeerCallback cb);

/// Callback lorsqu’un message de données est reçu.
void onMessageReceived(MessageCallback cb);

}
