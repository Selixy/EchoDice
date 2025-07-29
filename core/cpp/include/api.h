#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STATIC_API)
    #define API_Cpp
#elif defined(API_Cpp_EXPORTS)
    #define API_Cpp __declspec(dllexport)
#else
    #define API_Cpp __declspec(dllimport)  // ← cette ligne cause l’erreur si STATIC_API n’est pas défini
#endif

API_Cpp void network_Shutdown();
API_Cpp void network_GetCode();
API_Cpp void network_ConectTo(const char* remote_sdp);
API_Cpp bool network_SendMessage(const char* peer_id, const char* message);

API_Cpp void Set_ID(const char* id);

#ifdef __cplusplus
}
#endif
