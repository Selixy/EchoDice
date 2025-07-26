#pragma once

namespace network {

/// (Re)génère une OFFER SDP sur une PeerConnection singleton.
/// La SDP est imprimée en console via le callback `onLocalDescription`.
void createOffer();

} // namespace network
