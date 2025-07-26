// core/cpp/src/api.cpp

#include "api.h"                       // extern "C" + API_Cpp
#include "network/offer_logic.hpp"     // network::createOffer()
#include "network/connect_logic.hpp"   // network::connectTo()
#include "network/shutdown_logic.hpp"  // network::shutdown()

#include <string>

extern "C" {

/// Ferme proprement la PeerConnection
API_Cpp void network_Shutdown() {
    network::shutdown();
}

/// (Re)génère une OFFER SDP (print dans offer_logic.cpp)
API_Cpp void createOffer() {
    network::createOffer();
}

/// Applique une OFFER SDP reçue et génère une ANSWER SDP
API_Cpp void network_ConectTo(const char* remote_sdp) {
    if (remote_sdp) {
        network::connectTo(std::string(remote_sdp));
    }
}

} // extern "C"
