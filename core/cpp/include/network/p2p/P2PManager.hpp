#ifndef ECHODICE_P2PMANAGER_HPP
#define ECHODICE_P2PMANAGER_HPP

#include <string>
#include <functional>

namespace p2p {

using PeerCallback    = std::function<void(const std::string&)>;
using MessageCallback = std::function<void(const std::string&,
                                           const std::string&)>;

/// Haut niveau, façade unique pour l’API C.
class P2PManager {
public:
  static void init(const std::string& localId);
  static const std::string& getRoomCode();
  static void joinRoom(const std::string& code);
  static void send(const std::string& peerId,
                   const std::string& message);
  static void shutdown();

  static void onPeerJoined(PeerCallback cb);
  static void onMessageReceived(MessageCallback cb);
};

} // namespace p2p

#endif // ECHODICE_P2PMANAGER_HPP
