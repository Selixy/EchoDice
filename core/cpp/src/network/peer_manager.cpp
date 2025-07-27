#include "network/peer_manager.hpp"
#include <iostream>

namespace network {

void PeerManager::insert(const std::string& id,
                         std::shared_ptr<rtc::PeerConnection> pc)
{
    // 1) Abonnement aux changements d’état de la connexion
    pc->onStateChange([this, id](rtc::PeerConnection::State state) {
        switch (state) {
            case rtc::PeerConnection::State::New:
                break;

            case rtc::PeerConnection::State::Connecting:
                std::cout << "[network] Peer '" << id << "' is connecting…\n";
                break;

            case rtc::PeerConnection::State::Connected:
                std::cout << "[network] Peer '" << id
                          << "' is now CONNECTED\n";
                break;

            case rtc::PeerConnection::State::Disconnected:
            case rtc::PeerConnection::State::Failed:
            case rtc::PeerConnection::State::Closed:
            {
                std::lock_guard<std::mutex> lock(mtx_);
                peers_.erase(id);
                std::cout << "[network] Peer '" << id
                          << "' removed (state=" << state << ")\n";
                break;
            }
        }
    });

    // 2) Stockage sous la clé (thread-safe)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        peers_[id] = std::move(pc);
    }
}

std::shared_ptr<rtc::PeerConnection> PeerManager::get(const std::string& id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = peers_.find(id);
    return it != peers_.end() ? it->second : nullptr;
}

void PeerManager::remove(const std::string& id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = peers_.find(id);
    if (it != peers_.end()) {
        it->second->close();
        peers_.erase(it);
    }
}

void PeerManager::clear()
{
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto& kv : peers_) {
        kv.second->close();
    }
    peers_.clear();
}

void PeerManager::rename(const std::string& oldId, const std::string& newId)
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = peers_.find(oldId);
    if (it == peers_.end() || oldId == newId) return;

    // Si une entrée existe déjà sous newId, on la ferme pour éviter doublon
    if (auto dup = peers_.find(newId); dup != peers_.end()) {
        dup->second->close();
        peers_.erase(dup);
    }
    peers_[newId] = std::move(it->second);
    peers_.erase(it);

    std::cout << "[network] Peer renamed '" << oldId
              << "' → '" << newId << "'\n";
}

} // namespace network
