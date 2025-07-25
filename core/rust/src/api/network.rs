// src/api/network.rs

use crate::api::bindings;
use crate::api::command::CommandDesc;
use inventory;

// Initialise le sous-système réseau
pub fn init()     { unsafe { bindings::network_Init() } }
pub fn update()   { unsafe { bindings::network_Update() } }
pub fn shutdown() { unsafe { bindings::network_Shutdown() } }

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
        name:        "network/update",
        description: "faire un tick manuel du réseau",
        message:     "[CLI] réseau tick",
        callback:    update,
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