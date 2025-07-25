// src/api/bindings.rs

extern "C" {
    pub fn network_Init();
    pub fn network_Update();
    pub fn network_Shutdown();
}
