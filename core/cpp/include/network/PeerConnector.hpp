#pragma once
#include <string>
#include <functional>
#include "network/SignalCodec.hpp"
#include "network/PeerManager.hpp"

namespace signaling {

/// Applique l’offre distante (Base64), crée l’ANSWER encodé, le renvoie.
std::string connectToPeer(network::PeerManager& pm,
                          const std::string& myId,
                          const std::string& remoteCode);

/// Installe le handler pour l’ANSWER final reçu par message.
void setupMessageHandler(network::PeerManager& pm,
                         std::function<void(const std::string&)> onConnected);

} // namespace signaling
