// src/api/bindings.rs

extern "C" {
    pub fn network_Init();
    pub fn network_Shutdown();
    pub fn network_crate_offet();
}
