#include "network/p2p/P2PManager.hpp"
#include "network/p2p/RoomManager.hpp"

namespace p2p {

static std::unique_ptr<RoomManager> room;

void P2PManager::init(const std::string& localId) {
  room = std::make_unique<RoomManager>(localId);
}

const std::string& P2PManager::getRoomCode() {
  return room->createAndJoin();
}

void P2PManager::joinRoom(const std::string& code) {
  room->join(code);
}

void P2PManager::send(const std::string& peerId,
                      const std::string& message) {
  room->sendMessage(peerId, message);
}

void P2PManager::shutdown() {
  room->shutdown();
  room.reset();
}

void P2PManager::onPeerJoined(PeerCallback cb) {
  room->onPeerJoined(std::move(cb));
}

void P2PManager::onMessageReceived(MessageCallback cb) {
  room->onMessageReceived(std::move(cb));
}

} // namespace p2p
