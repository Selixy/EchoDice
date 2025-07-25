#pragma once

#ifdef _WIN32
  #ifdef API_Cpp_EXPORTS
    #define API_Cpp __declspec(dllexport)
  #else
    #define API_Cpp __declspec(dllimport)
  #endif
#else
  #define API_Cpp
#endif

extern "C" {

// … tes autres déclarations …

/// Appelée à chaque tick de la boucle Rust
API_Cpp void network_Update();

} // extern "C"
