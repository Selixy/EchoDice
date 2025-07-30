#pragma once
#ifdef API_Cpp_EXPORTS
# define API_Cpp __declspec(dllexport)
#else
# define API_Cpp __declspec(dllimport)
#endif

extern "C" {
    /// (Optionnel) fixe un ID avant GetCode()
    API_Cpp void Set_ID(const char* id);

    /// Génère un connection code (appelle l’API PeerJS HTTP) 
    /// et rejoint la room associée à ce code
    API_Cpp const char* network_GetCode();

    /// Rejoint la même room que network_GetCode (broadcast OFFER)
    API_Cpp void network_ConnectTo(const char* connection_code);

    /// Envoie un message texte sur la DataChannel P2P
    API_Cpp bool network_SendMessage(const char* peer_id, const char* message);

    /// Arrête proprement le sous-système P2P
    API_Cpp void network_Shutdown();
}
