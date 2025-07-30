#include "network/PeerJSWebSocket.hpp"
#include "Logger.hpp"
// ... inclure ta lib WebSocket boost/beast ici, ou utilise une fausse implémentation temporaire si besoin
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct PeerJSWebSocket::Impl {
    // TODO: ici, tu mets le vrai client WebSocket
    std::string host, key, id, token;
    int port;
    PeerJSWebSocket::OnOpen    onOpen;
    PeerJSWebSocket::OnMessage onMessage;

    Impl(const std::string& h, int p, const std::string& k,
         const std::string& i, const std::string& t)
        : host(h), port(p), key(k), id(i), token(t) {}

    void connect() {
        LOG_INFO("Connexion a PeerJS WS : " + host + ":" + std::to_string(port));
        // TODO: connection réelle (WebSocket handshake)
        if (onOpen) onOpen();
    }

    void send(const std::string& type,
              const std::string& dst,
              const std::string& payload) {
        // Fabrique un message PeerJS (type/src/dst/payload)
        json j;
        j["type"]    = type;
        j["src"]     = id;
        j["dst"]     = dst;
        j["payload"] = json::parse(payload);
        LOG_DEBUG("WS send : " + j.dump());
        // TODO: send via WebSocket
    }
};

PeerJSWebSocket::PeerJSWebSocket(const std::string& host,
                                 int port,
                                 const std::string& key,
                                 const std::string& id,
                                 const std::string& token)
    : impl_(std::make_unique<Impl>(host, port, key, id, token)) {}

PeerJSWebSocket::~PeerJSWebSocket() = default;

void PeerJSWebSocket::connect()            { impl_->connect(); }
void PeerJSWebSocket::send(const std::string& t,
                           const std::string& d,
                           const std::string& p) { impl_->send(t, d, p); }
void PeerJSWebSocket::setOnOpen(OnOpen cb)      { impl_->onOpen = std::move(cb); }
void PeerJSWebSocket::setOnMessage(OnMessage cb){ impl_->onMessage = std::move(cb); }
