#pragma once
#include <string>
#include "network/SignalCodec.hpp"
#include "network/PeerManager.hpp"   // votre PeerManager

namespace signaling {

/// Crée l’OFFER, l’encode ({id,sdp}→Base64) et renvoie la chaîne.
std::string getOfferCode(network::PeerManager& pm,
                         const std::string& myId);

} // namespace signaling
