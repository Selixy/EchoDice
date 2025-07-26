#pragma once
#include <string>

namespace network {

class PeerManager;

/// Crée un PeerConnection, génère une offre SDP, et l'insère dans le PeerManager.
void create_offer(PeerManager& manager, const std::string& id);

} // namespace network
