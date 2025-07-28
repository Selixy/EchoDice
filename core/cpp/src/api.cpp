#include "api.h"
#include "info.hpp"

#include "network/OfferManager.hpp"
#include "network/PeerConnector.hpp"
#include "network/SignalCodec.hpp"

#include <iostream>
#include <mutex>
using nlohmann::json;

// Ton PeerManager partagé
static network::PeerManager g_peerManager;

// Callback stocké + mutex pour la sécurité thread
static std::mutex g_cb_mutex;
static void (*g_message_cb)(const char* peer_id, const char* message) = nullptr;

extern "C" {

API_Cpp void Set_ID(const char* id) {
    if (id) gInfo.ID = id;
}

API_Cpp void network_GetCode() {
    if (gInfo.ID.empty()) {
        std::cerr << "[Error] ID not set. Call Set_ID() first.\n";
        return;
    }
    auto code = signaling::getOfferCode(g_peerManager, gInfo.ID);
    std::cout << "[Temporary Offer] " << code << std::endl;
}

API_Cpp void network_ConectTo(const char* remote_sdp) {
    if (!remote_sdp) {
        std::cerr << "[Error] remote_sdp is null.\n";
        return;
    }
    if (gInfo.ID.empty()) {
        std::cerr << "[Error] ID not set. Call Set_ID() first.\n";
        return;
    }

    // 1) Décoder pour extraire remoteId
    json j = signaling::decode(remote_sdp);
    std::string remoteId = j.at("id").get<std::string>();

    // 2) Générer l’ANSWER encodé
    auto answerCode = signaling::connectToPeer(
        g_peerManager, gInfo.ID, remote_sdp
    );

    // 3) Envoyer dès que le DataChannel est prêt
    g_peerManager.SendWhenReady(answerCode);
}

API_Cpp bool network_SendMessage(const char* peer_id,
                                 const char* message)
{
    if (!peer_id || !message) return false;
    return g_peerManager.SendMessage(peer_id, message);
}

API_Cpp void network_Shutdown() {
    g_peerManager.Close();
}

/// Enregistre un callback pour remonter les messages reçus
API_Cpp void network_SetOnMessage(void (*cb)(const char* peer_id,
                                             const char* message))
{
    std::lock_guard<std::mutex> lock(g_cb_mutex);
    g_message_cb = cb;

    // On (re)branche notre logique de réception
    g_peerManager.onMessage(
        [](const std::string& from, const std::string& payload) {
            // 1) Tentative de décodage JSON/Base64
            try {
                auto j = signaling::decode(payload);
                // Si on a bien un « sdp », on traite comme réponse SDP
                if (j.contains("sdp")) {
                    auto sdp = j.at("sdp").get<std::string>();
                    std::cout << "[api] Applying final ANSWER from " << from << "\n";
                    g_peerManager.SetRemoteDescription("answer", sdp);
                    return; // on n’appelle pas le callback Rust
                }
            } catch (...) {
                // payload n'était pas un JSON Base64 valide → c'est un chat normal
            }

            // 2) C'est un message "chat" : on le transmet à Rust
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            if (g_message_cb) {
                g_message_cb(from.c_str(), payload.c_str());
            }
        }
    );
}


} // extern "C"
