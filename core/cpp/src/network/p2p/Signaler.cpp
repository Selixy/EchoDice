#include "network/p2p/Signaler.hpp"

namespace p2p {

Signaler::Signaler(const std::string& host,
                   int                 port,
                   const std::string&  key,
                   const std::string&  id,
                   const std::string&  path)
  : socket_(std::make_unique<PeerJS::PeerJSWebSocket>(host, port, key, id, path))
{}

Signaler::~Signaler() {
  shutdown();
}

void Signaler::connect() {
  socket_->connect();
}

void Signaler::shutdown() {
  socket_.reset();
}

void Signaler::setOnOpen(OnOpen cb) {
  socket_->setOnOpen(std::move(cb));
}

void Signaler::setOnMessage(OnMessage cb) {
  socket_->setOnMessage(std::move(cb));
}

void Signaler::send(const std::string& type,
                    const std::string& dst,
                    const std::string& payload) {
  socket_->send(type, dst, payload);
}

} // namespace p2p
