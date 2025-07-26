#pragma once
#include <string>

namespace network {

/// Initialise WebRTC et crée l’offer (imprimée et stockée).
void init();

/// Ferme proprement la PeerConnection.
void shutdown();

/// Applique l’offre SDP reçue, génère l’answer SDP
/// (imprimée et stockée) et renvoie une référence vers la chaîne.
const std::string& connectTo(const std::string& remoteSdp);

/// Renvoie la dernière SDP générée (offer ou answer).
const std::string& lastSdp();

} // namespace network
