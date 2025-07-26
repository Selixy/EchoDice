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
    unsafe { bindings::createOffer() }
}

pub fn connect_to(offer_sdp: &str) {
    let c_offer: CString = CString::new(offer_sdp)
    .expect("Impossible de convertir l’offer en CString");
    unsafe {
        bindings::network_ConectTo(c_offer.as_ptr() as *const c_char);
    }
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