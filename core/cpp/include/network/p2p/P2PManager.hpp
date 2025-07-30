#pragma once

#include <string>
#include <functional>

namespace p2p {

/// Initialise le manager P2P local avec l’ID de ce client.
void initLocal(const std::string& id);

/// Génère (une seule fois) un code de room aléatoire, le retourne.
std::string getCode();

/// Se connecte à la room identifiée par ce code (abonnement au signaling).
void connectTo(const std::string& roomCode);

/// Envoie un message P2P à un pair donné.
void sendMessage(const std::string& peerId,
                 const std::string& message);

/// Détruit / ferme proprement le signaling.
void shutdown();

/// Callback appelé à chaque nouveau pair rejoint la room (avec son peerId).
using PeerCallback    = std::function<void(const std::string& peerId)>;
void onPeerJoined(PeerCallback cb);

/// Callback appelé à la réception d’un message P2P (src → message).
using MessageCallback = std::function<void(const std::string& src,
                                           const std::string& message)>;
void onMessageReceived(MessageCallback cb);

} // namespace p2p
