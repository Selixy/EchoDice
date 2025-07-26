#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "rtc/rtc.hpp"

namespace network {

/// Gère plusieurs PeerConnection identifiés par un peerId.
/// - createPeer()      → instancie le peer
/// - createOffer(id)   → génère/retourne l’offer SDP pour ce peer
/// - connectTo(id, sdp)→ applique l’offer et retourne l’answer SDP
/// - removePeer(id)    → ferme et supprime la connexion
/// - shutdownAll()     → ferme tous les peers
class PeerManager {
public:
    void createPeer(const std::string& peerId);
    std::string createOffer(const std::string& peerId);
    std::string connectTo(const std::string& peerId, const std::string& remoteSdp);
    void removePeer(const std::string& peerId);
    void shutdownAll();

private:
    struct Peer {
        std::shared_ptr<rtc::PeerConnection> pc;
        std::mutex                          mtx;
        std::string                         lastSdp;
    };

    std::unordered_map<std::string, std::unique_ptr<Peer>> peers_;
    std::mutex                                             mapMtx_;

    void ensurePeerInitialized(Peer& peer);
};

} // namespace network
