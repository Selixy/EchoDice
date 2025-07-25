#pragma once

namespace network {

/// Initialise tout ce qu’il faut pour le subsystem réseau.
/// Appelle une seule fois au démarrage.
void init();

/// Exécuté à chaque tick de la boucle Rust.
void tick();

/// Détruit / nettoie les ressources réseau.
void shutdown();

} // namespace network
