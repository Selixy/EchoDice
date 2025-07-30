#include "network/Signaler.hpp"

Signaler::Signaler(const std::string& host,
                   int port,
                   const std::string& key,
                   const std::string& id,
                   const std::string& path)
    : socket_(std::make_unique<PeerJSWebSocket>(host, port, key, id, path))
{}

Signaler::~Signaler() { shutdown(); }

void Signaler::connect()                          { socket_->connect(); }
void Signaler::send(const std::string& t,
                    const std::string& d,
                    const std::string& p)         { socket_->send(t, d, p); }
void Signaler::setOnOpen(OnOpen cb)               { socket_->setOnOpen(std::move(cb)); }
void Signaler::setOnMessage(OnMessage cb)         { socket_->setOnMessage(std::move(cb)); }
void Signaler::shutdown()                         { socket_.reset(); }
