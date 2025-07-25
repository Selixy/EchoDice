// core/cpp/src/api.cpp

#include "api.h"

#include <rtc/rtc.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>

// Indique si la collecte ICE est terminée
static std::atomic<bool> g_gathering_complete{false};

// Callback C appelé lors du changement d'état de ICE
static void on_gathering_state(int /*pc*/, rtcGatheringState state, void* /*user_ptr*/) {
    if (state == RTC_GATHERING_COMPLETE) {
        g_gathering_complete.store(true);
    }
}

extern "C" {

/// Génère une offre SDP et renvoie un buffer mallocé contenant le SDP.
/// L'appelant doit libérer cette mémoire en appelant free_sdp().
API_NETWORK_API char* generate_sdp_offer() {
    // Réinitialiser l'état de gathering
    g_gathering_complete.store(false);

    // 1) Initialisation du logger (optionnel) et préchargement des ressources
    rtcInitLogger(RTC_LOG_INFO, nullptr);
    rtcPreload();

    // 2) Configuration ICE avec un serveur STUN public
    rtcConfiguration config{};
    const char* ice_servers[] = { "stun:stun.l.google.com:19302" };
    config.iceServers      = ice_servers;
    config.iceServersCount = 1;

    // 3) Création de la PeerConnection et enregistrement du callback
    int pc = rtcCreatePeerConnection(&config);
    rtcSetGatheringStateChangeCallback(pc, on_gathering_state);

    // 4) Création d'un DataChannel nommé "data"
    //    Nouvelle API : rtcCreateDataChannel(pc, label)
    rtcCreateDataChannel(pc, "data");

    // 5) Attendre la fin du gathering ICE
    while (!g_gathering_complete.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // 6) Récupérer la description locale (SDP)
    int len = rtcGetLocalDescription(pc, nullptr, 0);
    char* sdp = nullptr;
    if (len > 0) {
        sdp = static_cast<char*>(std::malloc(len));
        rtcGetLocalDescription(pc, sdp, len);
    } else {
        // En cas d'erreur, retourner une chaîne vide
        sdp = static_cast<char*>(std::malloc(1));
        sdp[0] = '\0';
    }

    // 7) Nettoyage de la PeerConnection et des ressources globales
    rtcClosePeerConnection(pc);
    rtcDeletePeerConnection(pc);
    rtcCleanup();

    return sdp;
}

/// Libère le buffer alloué par generate_sdp_offer()
API_NETWORK_API void free_sdp(char* ptr) {
    std::free(ptr);
}

} // extern "C"
