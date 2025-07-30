#pragma once
#include <functional>
#include <string>
#include <memory>

namespace PeerJS {
  using RawMessageCallback = std::function<void(const std::string&)>;

  class WebSocketClient {
  public:
    WebSocketClient(const std::string& host,
                    const std::string& port,
                    const std::string& target);
    ~WebSocketClient();

    // démarre la connexion (bloquant)
    void connect();
    // envoie un texte
    void send(const std::string& message);
    // callback pour chaque message texte reçu
    void onMessage(RawMessageCallback cb);
    // arrête proprement
    void stop();
  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
  };
}
