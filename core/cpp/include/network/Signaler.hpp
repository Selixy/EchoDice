#pragma once
#include <string>
#include <functional>
#include <memory>
#include "network/PeerJSWebSocket.hpp"

class Signaler {
public:
    using OnOpen    = std::function<void()>;
    using OnMessage = std::function<void(const std::string& type,
                                         const std::string& src,
                                         const std::string& payload)>;

    Signaler(const std::string& host,
             int port,
             const std::string& key,
             const std::string& id,
             const std::string& path);
    ~Signaler();

    void connect();
    void send(const std::string& type,
              const std::string& dst,
              const std::string& payload);

    void setOnOpen(OnOpen cb);
    void setOnMessage(OnMessage cb);

    void shutdown();

private:
    std::unique_ptr<PeerJSWebSocket> socket_;
};
