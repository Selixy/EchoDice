#pragma once
#include <string>

namespace network {

/// Applique la OFFER SDP reçue, génère une ANSWER SDP,
/// puis l’imprime en console via le callback `onLocalDescription`.
void connectTo(const std::string& remoteSdp);

} // namespace network
