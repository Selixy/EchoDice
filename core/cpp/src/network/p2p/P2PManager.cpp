#include "network/p2p/P2PManager.hpp"
#include "network/serveur/PeerJSWebSocket.hpp"
#include <nlohmann/json.hpp>
#include <memory>

using json = nlohmann::json;

namespace p2p {
static std::unique_ptr<PeerJS::PeerJSWebSocket> signal;
static std::string localId;

void initLocal(const std::string& id) {
  localId = id;
  signal = std::make_unique<PeerJS::PeerJSWebSocket>(
    "0.peerjs.com", 443, "peerjs", localId, "auto");
}

std::string getCode() {
  return localId;
}

void connectTo(const std::string& code) {
  if (!signal) initLocal(code);
  signal->setOnOpen([&](){
    // onOpen on ne fait rien de plus ici
  });
  signal->connect();
}

void sendMessage(const std::string& peerId,
                 const std::string& message)
{
  json j;
  j["message"] = message;
  signal->send("MESSAGE", peerId, j.dump());
}

void shutdown() {
  signal.reset();
}

} // namespace p2p
