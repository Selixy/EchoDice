#include "network/peer_manager.hpp"

namespace network {

void PeerManager::insert(const std::string& id, std::shared_ptr<rtc::PeerConnection> peer) {
    std::lock_guard<std::mutex> lock(mtx_);
    peers_[id] = std::move(peer);
}

std::shared_ptr<rtc::PeerConnection> PeerManager::get(const std::string& id) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = peers_.find(id);
    return it != peers_.end() ? it->second : nullptr;
}

void PeerManager::remove(const std::string& id) {
    std::lock_guard<std::mutex> lock(mtx_);
    peers_.erase(id);
}

void PeerManager::clear() {
    std::lock_guard<std::mutex> lock(mtx_);
    peers_.clear();
}

} // namespace network
