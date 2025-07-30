#ifndef ECHODICE_ROOMMANAGER_HPP
#define ECHODICE_ROOMMANAGER_HPP

#include "network/p2p/Signaler.hpp"
#include <functional>
#include <string>

namespace p2p {

using PeerCallback    = std::function<void(const std::string& peerId)>;
using MessageCallback = std::function<void(const std::string& from,
                                           const std::string& message)>;

/// Gère la création/join d’une “room” et notifie le reste de l’app.
class RoomManager {
public:
  explicit RoomManager(const std::string& localId);

  /// Crée une room (code aléatoire) et s’y inscrit.
  const std::string& createAndJoin();

  /// Rejoint explicitement une room existante.
  void join(const std::string& roomCode);

  /// Envoie un message direct à peerId.
  void sendMessage(const std::string& peerId,
                   const std::string& message);

  /// Callbacks pour un pair qui rejoint / messages reçus.
  void onPeerJoined(PeerCallback cb);
  void onMessageReceived(MessageCallback cb);

  /// Ferme la connexion signal et réinitialise l’état.
  void shutdown();

private:
  std::string generateRoomCode();

  std::string            localId_;
  std::string            roomCode_;
  Signaler               signaler_;
  PeerCallback           peerJoinedCb_;
  MessageCallback        messageCb_;
};

} // namespace p2p

#endif // ECHODICE_ROOMMANAGER_HPP
