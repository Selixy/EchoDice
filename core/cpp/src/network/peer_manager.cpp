#include "network/peer_manager.hpp"

namespace network {

void PeerManager::insert(const std::string& id,
                         std::shared_ptr<rtc::PeerConnection> pc) {
    // 1) On s'abonne aux changements d'état de la PeerConnection
    pc->onStateChange([this, id](rtc::PeerConnection::State state) {
        switch (state) {
            case rtc::PeerConnection::State::New:
                // optionnel : juste créé
                break;

            case rtc::PeerConnection::State::Connecting:
                std::cout << "[network] Peer '" << id << "' is connecting…\n";
                break;

            case rtc::PeerConnection::State::Connected:
                // ← **Connexion établie !**
                std::cout << "[network] Peer '" << id 
                          << "' is now CONNECTED\n";
                break;

            case rtc::PeerConnection::State::Disconnected:
            case rtc::PeerConnection::State::Failed:
            case rtc::PeerConnection::State::Closed:
                {
                    std::lock_guard<std::mutex> lock(mtx_);
                    peers_.erase(id);
                }
                std::cout << "[network] Peer '" << id 
                          << "' removed (state=" << state << ")\n";
                break;
        }
    });

    // 2) On stocke la PeerConnection dans la map
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
