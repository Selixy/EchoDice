// core/cpp/src/api.cpp

#include "api.h"
#include "info.hpp"
#include "network/connect_offer.hpp"
#include "network/peer_manager.hpp"
#include "network/create_offer.hpp"
#include "network/send_message.hpp"
#include "network/sdp_codec.hpp"
#include "network/global.hpp"  

#include <string>

static network::PeerManager g_peerManager;

extern "C" {

/// Ferme proprement la PeerConnection
API_Cpp void network_Shutdown() {
    g_peerManager.clear();
}

/// génère une OFFER SDP
API_Cpp void network_GetCode() {
    network::create_offer(g_peerManager);
}

/// Applique une OFFER SDP reçue et génère une ANSWER SDP
API_Cpp void network_ConectTo(const char* remote_sdp) {
    if (!remote_sdp) return;

    try {
        std::string decoded = network::decode_sdp(remote_sdp);
        network::connect_to_offer(g_peerManager, decoded);
    } catch (const std::exception& e) {
        std::cerr << "[network] Echec dans network_ConectTo : " << e.what() << "\n";
    }
}

API_Cpp bool network_SendMessage(const char* peer_id,
                                 const char* message)
{
    if (!peer_id || !message) return false;
    return network::send_message(network::gPeerManager,
                                 peer_id,
                                 message);
}

/// Defini l'ID
API_Cpp void Set_ID(const char* id) {
    if (id) {
        gInfo.ID = std::string(id);
    }
}

} // extern "C"
