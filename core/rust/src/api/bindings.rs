// src/api/bindings.rs

use std::os::raw::c_char;

extern "C" {
    pub fn network_Shutdown();
    pub fn network_GetCode();
    pub fn network_ConnectTo(remote_sdp: *const c_char);
    pub fn network_SendMessage(peer_id: *const c_char,
                               message:   *const c_char) -> bool;


    pub fn Set_ID(id: *const c_char);
}
