#pragma once

#ifdef _WIN32
  #ifdef API_NETWORK_EXPORTS
    #define API_NETWORK_API __declspec(dllexport)
  #else
    #define API_NETWORK_API __declspec(dllimport)
  #endif
#else
  #define API_NETWORK_API
#endif

extern "C" {

// … tes autres déclarations …

/// Appelée à chaque tick de la boucle Rust
API_NETWORK_API void network_Update();

} // extern "C"
