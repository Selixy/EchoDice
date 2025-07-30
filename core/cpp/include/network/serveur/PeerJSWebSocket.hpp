#pragma once
#include "network/serveur/WebSocketClient.hpp"
#include <functional>
#include <string>
#include <memory>

namespace PeerJS {
  using OnOpen    = std::function<void()>;
  using OnMessage = std::function<void(const std::string& type,
                                       const std::string& src,
                                       const std::string& payload)>;

  class PeerJSWebSocket {
  public:
    PeerJSWebSocket(const std::string& host,
                    int port,
                    const std::string& key,
                    const std::string& id,
                    const std::string& token);
    ~PeerJSWebSocket();

    // identifiant local / "code"
    const std::string& getId() const;

    void setOnOpen(OnOpen cb);
    void setOnMessage(OnMessage cb);

    void connect(); 
    void send(const std::string& type,
              const std::string& dst,
              const std::string& payload);

  private:
    std::string host_, key_, id_, token_, target_;
    int         port_;
    OnOpen      onOpen_;
    OnMessage   onMessage_;
    std::unique_ptr<WebSocketClient> ws_;
  };
}
