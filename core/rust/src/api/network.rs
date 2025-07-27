// src/api/network.rs

use crate::api::bindings;
use crate::api::command::CommandDesc;

use std::os::raw::c_char;
use std::ffi::CString;
use inventory;



pub fn shutdown() {
    unsafe { bindings::network_Shutdown() }
}

pub fn GetCode() {
    unsafe { bindings::network_GetCode() }
}

pub fn connect_to(offer_sdp: &str) {
    let c_offer: CString = CString::new(offer_sdp)
    .expect("Impossible de convertir l’offer en CString");
    unsafe {
        bindings::network_ConectTo(c_offer.as_ptr() as *const c_char);
    }
}

pub fn send_message(peer_id: &str, msg: &str) -> bool {
    let c_peer = CString::new(peer_id)
        .expect("peer_id contient un NUL byte");
    let c_msg  = CString::new(msg)
        .expect("message contient un NUL byte");
    unsafe { bindings::network_SendMessage(c_peer.as_ptr(), c_msg.as_ptr()) }
}


inventory::submit! {
    CommandDesc {
        name:        "network/shutdown",
        description: "arrêter le sous-système réseau",
        message:     "[network] réseau arrêté",
        callback:    |_arg| shutdown(),
    }
}

inventory::submit! {
    CommandDesc {
        name:        "network/GetCode",
        description: "créer une offre SDP",
        message:     "[network] offre SDP créée",
        callback:    |_arg| GetCode(),
    }
}

inventory::submit! {
    CommandDesc {
        name:        "network/connectTo",
        description: "appliquer une offer SDP et générer l’answer",
        message:     "[network] connectTo avec paramètre",
        callback:    |opt| {
            if let Some(sdp) = opt {
                connect_to(sdp);
            } else {
                eprintln!("network/connectTo attend un paramètre SDP");
            }
        },
    }
}

inventory::submit! {
    CommandDesc {
        name:        "network/sendMessage",
        description: "envoyer un message au pair donné",
        message:     "[network] message envoyé",
        callback:    |opt| {
            if let Some(args) = opt {
                // Sépare peerId et payload (premier mot = ID, le reste = msg)
                if let Some((id, msg)) = args.split_once(' ') {
                    if !send_message(id, msg) {
                        eprintln!("[network] échec d'envoi (peer inconnu ?)");
                    }
                } else {
                    eprintln!("Usage : network/sendMessage <peerId> <message>");
                }
            } else {
                eprintln!("network/sendMessage attend deux paramètres");
            }
        },
    }
}