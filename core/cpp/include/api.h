#pragma once

#ifdef _WIN32
  #ifdef API_Network_EXPORTS    // CMake injecte -DAPI_Network_EXPORTS
    #define API_NETWORK_API __declspec(dllexport)
  #else
    #define API_NETWORK_API __declspec(dllimport)
  #endif
#else
  #define API_NETWORK_API
#endif

extern "C" {
  API_NETWORK_API char* generate_sdp_offer();
  API_NETWORK_API void  free_sdp(char* ptr);
}
