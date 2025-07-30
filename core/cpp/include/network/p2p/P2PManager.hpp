// network/p2p/P2PManager.hpp
#pragma once

#include <string>
#include <functional>

namespace p2p {
  using PeerCallback    = std::function<void(const std::string& peerId)>;
  using MessageCallback = std::function<void(const std::string& peerId, const std::string& message)>;

  void initLocal(const std::string& id);
  std::string getCode();
  void connectTo(const std::string& roomCode);
  void sendMessage(const std::string& peerId, const std::string& message);
  void shutdown();

  void onPeerJoined(PeerCallback cb);
  void onMessageReceived(MessageCallback cb);
}
