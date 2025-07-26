// core/cpp/src/api.cpp

#include "api.h"
#include "network/peer_manager.hpp"
#include "network/create_offer.hpp"
#include "network/connect_offer.hpp"
#include "network/sdp_codec.hpp"

#include <string>

static network::PeerManager g_peerManager; // ✅ Ajout nécessaire

extern "C" {

/// Ferme proprement la PeerConnection
API_Cpp void network_Shutdown() {
    g_peerManager.clear();  // ✅ nettoie tous les PeerConnection
}

/// génère une OFFER SDP
API_Cpp void createOffer() {
    network::create_offer(g_peerManager, "peer1");
}

/// Applique une OFFER SDP reçue et génère une ANSWER SDP
API_Cpp void network_ConectTo(const char* remote_sdp) {
    if (!remote_sdp) return;

    try {
        std::string decoded = network::decode_sdp(remote_sdp);
        network::connect_to_offer(g_peerManager, "peer2", decoded);
    } catch (const std::exception& e) {
        std::cerr << "[network] Echec dans network_ConectTo : " << e.what() << "\n";
    }
}

} // extern "C"
