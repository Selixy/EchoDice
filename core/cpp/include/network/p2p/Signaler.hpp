#ifndef ECHODICE_SIGNALER_HPP
#define ECHODICE_SIGNALER_HPP

#include "network/serveur/PeerJSWebSocket.hpp"
#include <functional>
#include <memory>
#include <string>

namespace p2p {

/// Enveloppe autour de PeerJSWebSocket pour exposer juste ce dont on a besoin.
class Signaler {
public:
  using OnOpen    = std::function<void()>;
  using OnMessage = std::function<void(const std::string& type,
                                       const std::string& src,
                                       const std::string& payload)>;

  Signaler(const std::string& host,
           int                 port,
           const std::string&  key,
           const std::string&  id,
           const std::string&  path);
  ~Signaler();

  /// Ouvre la connexion WebSocket.
  void connect();

  /// Ferme proprement la connexion et détruit le socket.
  void shutdown();

  /// Callbacks pour événement d’ouverture et réception de message.
  void setOnOpen(OnOpen cb);
  void setOnMessage(OnMessage cb);

  /// Envoie un signal de type/type à la cible dst.
  void send(const std::string& type,
            const std::string& dst,
            const std::string& payload);

private:
  std::unique_ptr<PeerJS::PeerJSWebSocket> socket_;
};

} // namespace p2p

#endif // ECHODICE_SIGNALER_HPP
