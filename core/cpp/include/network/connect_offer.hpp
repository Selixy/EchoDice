#pragma once
#include <string>

namespace network {

class PeerManager;

/// Crée un PeerConnection, consomme un SDP distant, génère une answer, et insère dans PeerManager.
void connect_to_offer(PeerManager& manager, const std::string& id, const std::string& remoteSdp);

} // namespace network
