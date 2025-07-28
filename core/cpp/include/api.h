#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define API_Cpp __declspec(dllexport)

API_Cpp void network_Shutdown();
API_Cpp void network_GetCode();
API_Cpp void network_ConectTo(const char* remote_sdp);
API_Cpp bool network_SendMessage(const char* peer_id, const char* message);
API_Cpp void network_SetOnMessage(void (*cb)(const char* peer_id, const char* message));

API_Cpp void Set_ID(const char* id);

#ifdef __cplusplus
}
#endif
