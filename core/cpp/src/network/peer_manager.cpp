#include "network/peer_manager.hpp"

namespace network {

void PeerManager::insert(const std::string& id,
                         std::shared_ptr<rtc::PeerConnection> pc) {
    // 1) On s'abonne aux changements d'état de la PeerConnection
    pc->onStateChange([this, id](rtc::PeerConnection::State state) {
        // Dès que l'état devient terminal, on supprime le peer
        if (state == rtc::PeerConnection::State::Disconnected ||
            state == rtc::PeerConnection::State::Failed       ||
            state == rtc::PeerConnection::State::Closed) {
            
            std::lock_guard<std::mutex> lock(mtx_);
            peers_.erase(id);
            std::cout << "[network] Peer '" << id 
                      << "' removed (state=" << state << ")\n";
        }
    });

    {
        std::lock_guard<std::mutex> lock(mtx_);
        peers_[id] = std::move(pc);
    }
}


std::shared_ptr<rtc::PeerConnection> PeerManager::get(const std::string& id) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = peers_.find(id);
    return it != peers_.end() ? it->second : nullptr;
}

void PeerManager::remove(const std::string& id) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = peers_.find(id);
    if (it != peers_.end()) {
        it->second->close();
        peers_.erase(it);
    }
}

void PeerManager::clear() {
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto& kv : peers_) {
        kv.second->close();
    }
    peers_.clear();
}

} // namespace network
