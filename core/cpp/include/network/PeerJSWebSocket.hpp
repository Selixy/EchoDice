#pragma once
#include <string>
#include <functional>
#include <memory>

class PeerJSWebSocket {
public:
    using OnOpen    = std::function<void()>;
    using OnMessage = std::function<void(const std::string& type,
                                         const std::string& src,
                                         const std::string& payload)>;

    PeerJSWebSocket(const std::string& host,
                    int port,
                    const std::string& key,
                    const std::string& id,
                    const std::string& token);
    ~PeerJSWebSocket();

    void connect();
    void send(const std::string& type,
              const std::string& dst,
              const std::string& payload);

    void setOnOpen(OnOpen cb);
    void setOnMessage(OnMessage cb);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
