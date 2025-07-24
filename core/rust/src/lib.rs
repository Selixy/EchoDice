use std::ffi::CStr;

#[link(name = "echo_dice", kind = "static")]
extern "C" {
    fn Test_WebRTC() -> *const std::os::raw::c_char;
}


pub fn test_webrtc() -> &'static str {
    unsafe {
        let c_str = Test_WebRTC();
        CStr::from_ptr(c_str).to_str().expect("Invalid UTF-8")
    }
}

