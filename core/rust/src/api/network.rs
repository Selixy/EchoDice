// src/api/network.rs

use crate::api::bindings;
use crate::api::command::CommandDesc;
use inventory;

// Initialise le sous-système réseau
pub fn init()     { unsafe { bindings::network_Init() } }
pub fn shutdown() { unsafe { bindings::network_Shutdown() } }
pub fn offer()    { unsafe { bindings::network_crate_offet() } }

// commandes
inventory::submit! {
    CommandDesc {
        name:        "network/init",
        description: "initialiser le sous-système réseau",
        message:     "[CLI] réseau initialisé",
        callback:    init,
    }
}

inventory::submit! {
    CommandDesc {
        name:        "network/shutdown",
        description: "arrêter le sous-système réseau",
        message:     "[CLI] réseau arrêté",
        callback:    shutdown,
    }
}

inventory::submit! {
    CommandDesc {
        name:        "network/offet",
        description: "creer une offre",
        message:     "creation d une offre...",
        callback:    shutdown,
    }
}