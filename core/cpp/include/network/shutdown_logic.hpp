#pragma once

namespace network {

/// Ferme proprement la PeerConnection et libère les ressources.
/// (à implémenter si tu veux réinitialiser ou détruire le PC)
void shutdown();

} // namespace network
