#pragma once
#include <string>
#include <memory>
#include <functional>
#include "network/Signaler.hpp"

using PeerJoinedCallback = std::function<void(const std::string& peerId)>;

class RoomManager {
public:
    RoomManager(const std::string& localId);

    const std::string& createAndJoin();
    const std::string& getRoomCode() const;
    void join(const std::string& roomCode);
    void shutdown();

    // Nouveau : permet d’être notifié d’un peer qui rejoint
    void onPeerJoined(PeerJoinedCallback cb);

private:
    std::string generateRoomCode();

    std::string localId_;
    std::string roomCode_;
    std::unique_ptr<Signaler> signaler_;
    PeerJoinedCallback peerJoinedCb_; // <====

    void handleSignalingMessage(const std::string& type,
                                const std::string& src,
                                const std::string& payload);
};
