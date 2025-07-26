#include "network/peer_manager.hpp"
#include <stdexcept>
#include <cstdio>

namespace network {

void PeerManager::createPeer(const std::string& peerId) {
    std::lock_guard<std::mutex> lock(mapMtx_);
    if (peers_.count(peerId)) return;
    auto peer = std::make_unique<Peer>();
    ensurePeerInitialized(*peer);
    peers_.emplace(peerId, std::move(peer));
}

std::string PeerManager::createOffer(const std::string& peerId) {
    {
        std::lock_guard<std::mutex> lock(mapMtx_);
        if (!peers_.count(peerId)) {
            auto peer = std::make_unique<Peer>();
            ensurePeerInitialized(*peer);
            peers_.emplace(peerId, std::move(peer));
        }
    }
    Peer& peer = *peers_.at(peerId);
    std::lock_guard<std::mutex> lock(peer.mtx);
    peer.pc->setLocalDescription();
    return peer.lastSdp;
}

std::string PeerManager::connectTo(const std::string& peerId,
                                   const std::string& remoteSdp) {
    {
        std::lock_guard<std::mutex> lock(mapMtx_);
        if (!peers_.count(peerId)) {
            auto peer = std::make_unique<Peer>();
            ensurePeerInitialized(*peer);
            peers_.emplace(peerId, std::move(peer));
        }
    }
    Peer& peer = *peers_.at(peerId);
    std::lock_guard<std::mutex> lock(peer.mtx);
    peer.pc->setRemoteDescription(rtc::Description(remoteSdp));
    peer.pc->setLocalDescription();
    return peer.lastSdp;
}

void PeerManager::removePeer(const std::string& peerId) {
    std::lock_guard<std::mutex> lock(mapMtx_);
    auto it = peers_.find(peerId);
    if (it != peers_.end()) {
        it->second->pc->close();
        peers_.erase(it);
    }
}

void PeerManager::shutdownAll() {
    std::lock_guard<std::mutex> lock(mapMtx_);
    for (auto& kv : peers_) {
        kv.second->pc->close();
    }
    peers_.clear();
}

void PeerManager::ensurePeerInitialized(Peer& peer) {
    if (peer.pc) return;

    rtc::InitLogger(rtc::LogLevel::Info);
    rtc::Preload();

    rtc::Configuration config;
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");

    peer.pc = std::make_shared<rtc::PeerConnection>(config);
    peer.pc->onLocalDescription([&peer](rtc::Description desc) {
        std::lock_guard<std::mutex> lock(peer.mtx);
        peer.lastSdp = static_cast<std::string>(desc);
        std::printf("\n=== SDP for '%s' ===\n%s\n====================\n",
                    peerId.c_str(), peer.lastSdp.c_str());
    });
    peer.pc->createDataChannel("data");
}

} // namespace network
