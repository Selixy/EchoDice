#include "network/serveur/PeerJSWebSocket.hpp"
#include "Logger.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
using json = nlohmann::json;

namespace PeerJS {

PeerJSWebSocket::PeerJSWebSocket(const std::string& host,
                                 int port,
                                 const std::string& key,
                                 const std::string& id,
                                 const std::string& token)
  : host_(host)
  , port_(port)
  , key_(key)
  , id_(id)
  , token_(token)
{
  std::ostringstream oss;
  oss << "/peerjs?key=" << key_ << "&id=" << id_ << "&token=" << token_;
  target_ = oss.str();
}

PeerJSWebSocket::~PeerJSWebSocket() = default;

const std::string& PeerJSWebSocket::getId() const {
  return id_;
}

void PeerJSWebSocket::setOnOpen(OnOpen cb) {
  onOpen_ = std::move(cb);
}

void PeerJSWebSocket::setOnMessage(OnMessage cb) {
  onMessage_ = std::move(cb);
}

void PeerJSWebSocket::connect() {
  ws_ = std::make_unique<WebSocketClient>(
    host_, std::to_string(port_), target_);

  ws_->onMessage([this](const std::string& raw){
    auto j = json::parse(raw);
    auto type = j.value("type", "");
    if (type == "OPEN") {
      if (onOpen_) onOpen_();
    } else {
      std::string src     = j.value("src","");
      std::string payload = j["payload"].dump();
      if (onMessage_) onMessage_(type, src, payload);
    }
  });

  ws_->connect();
}

void PeerJSWebSocket::send(const std::string& type,
                           const std::string& dst,
                           const std::string& payload)
{
  json j;
  j["type"]    = type;
  j["src"]     = id_;
  j["dst"]     = dst;
  j["payload"] = json::parse(payload);
  ws_->send(j.dump());
}

} // namespace PeerJS
