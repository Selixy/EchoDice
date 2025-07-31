// network/RoomManager.hpp
#pragma once

#include <string>
#include <functional>
#include <memory>
#include "network/Signaler.hpp"
#include "network/PeerManager.hpp"

using PeerJoinedCallback = std::function<void(const std::string& peerId)>;

class RoomManager {
public:
    explicit RoomManager(const std::string& localId);

    // Cree une room (code) et se connecte en signalisation
    const std::string& createAndJoin();

    // Rejoint une room existante
    void join(const std::string& code);

    // Retourne le code courant
    const std::string& getRoomCode() const;

    // Ferme proprement la signalisation et reinitialise
    void shutdown();

    // Callback quand un peer se connecte en P2P
    void onPeerJoined(PeerJoinedCallback cb);

private:
    // Genere un code aleatoire
    std::string generateRoomCode();

    // Initialise PeerManager pour SDP/ICE
    void setupPeerManager();

    // Traite tous les messages de signalisation
    void handleSignalingMessage(const std::string& type,
                                const std::string& src,
                                const std::string& payload);

    std::string                        localId_;
    std::string                        roomCode_;
    std::unique_ptr<Signaler>         signaler_;
    std::unique_ptr<PeerManager>      peerMgr_;
    PeerJoinedCallback                peerJoinedCb_;
};
