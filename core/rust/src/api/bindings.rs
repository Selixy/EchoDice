// src/api/bindings.rs

use std::os::raw::c_char;

extern "C" {
    pub fn network_Init();
    pub fn network_Shutdown();
    pub fn network_ConectTo(remote_sdp: *const c_char);
}
