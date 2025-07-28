// src/api/handler_message.rs

use crate::api::bindings;
use std::ffi::CStr;
use std::os::raw::c_char;

extern "C" fn on_message(peer_id: *const c_char, message: *const c_char) {
    if peer_id.is_null() || message.is_null() {
        eprintln!("[Rust] on_message: null pointer received");
        return;
    }
    let id = unsafe { CStr::from_ptr(peer_id) }
        .to_string_lossy()
        .into_owned();
    let msg = unsafe { CStr::from_ptr(message) }
        .to_string_lossy()
        .into_owned();

    // Affiche le message reçu
    println!("\n[Reçu de {}] {}\n", id, msg);
}

pub fn init_message_handler() {
    unsafe {
        bindings::network_SetOnMessage(on_message);
    }
}
