#![allow(dead_code)]   // optionnel, pour éviter des warnings

#[link(name = "echodice_api")]
extern "C" {
    fn generate_sdp_offer() -> *mut std::os::raw::c_char;
    fn free_sdp(ptr: *mut std::os::raw::c_char);
}

use std::ffi::CStr;

/// Génère une offre SDP via la DLL `echodice_api`.
pub fn get_sdp_offer() -> String {
    unsafe {
        let ptr = generate_sdp_offer();
        if ptr.is_null() {
            return String::new();
        }
        let s = CStr::from_ptr(ptr).to_string_lossy().into_owned();
        free_sdp(ptr);
        s
    }
}
