// src/api/network.rs

use crate::api::bindings;
use crate::api::command::CommandDesc;
use inventory;

pub fn init() {
    unsafe { bindings::network_Init() }
}

pub fn shutdown() {
    unsafe { bindings::network_Shutdown() }
}

pub fn offer() {
    unsafe { bindings::network_Init() }
}

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
        name:        "network/offer",
        description: "créer une offre SDP",
        message:     "[CLI] offre SDP créée",
        callback:    offer,
    }
}
