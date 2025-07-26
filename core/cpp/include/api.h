// include/network/api.h
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
  API_Cpp void    network_Init();
  API_Cpp void    network_Shutdown();
  API_Cpp void    network_ConectTo(const char* remote_sdp);
}
