#include "api.h"
#include "info.hpp"

#include "network/OfferManager.hpp"
#include "network/PeerConnector.hpp"
#include "network/SignalCodec.hpp"

#include <iostream>
#include <mutex>

using nlohmann::json;

static network::PeerManager g_peerManager;
static std::mutex g_cb_mutex;
static void (*g_message_cb)(const char* peer_id, const char* message) = nullptr;

extern "C" {

API_Cpp void Set_ID(const char* id) {
    if (id) {
        gInfo.ID = id;
    }
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

    // 1) Décoder pour extraire remoteId (non utilisé ici mais pour debug)
    json j = signaling::decode(remote_sdp);
    std::string remoteId = j.at("id").get<std::string>();
    std::cout << "[api] connectToPeer for remoteId = " << remoteId << "\n";

    // 2) Générer l’ANSWER encodé
    auto answerCode = signaling::connectToPeer(
        g_peerManager, gInfo.ID, remote_sdp
    );

    // 3) Envoyer dès que le DataChannel sera prêt (ou stocker en attendant)
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

/// Enregistre un callback Rust pour les messages reçus.
/// Applique automatiquement l’ANSWER (JSON Base64) avant d’appeler le callback.
API_Cpp void network_SetOnMessage(void (*cb)(const char* peer_id,
                                             const char* message))
{
    std::lock_guard<std::mutex> lock(g_cb_mutex);
    g_message_cb = cb;

    // (Re)branche la réception
    g_peerManager.onMessage(
        [](const std::string& from, const std::string& payload) {
            // Tenter de décoder comme JSON/Base64 {id,sdp}
            try {
                auto j = signaling::decode(payload);
                if (j.contains("sdp")) {
                    auto sdp = j.at("sdp").get<std::string>();
                    std::cout << "[api] Applying final ANSWER from " << from << "\n";
                    g_peerManager.SetRemoteDescription("answer", sdp);
                    return; // on n’appelle pas le callback Rust pour un SDP
                }
            } catch (...) {
                // ce n’était pas un SDP encodé → on traite comme message chat
            }

            // C’est un message “chat” normal : on l’envoie à Rust
            std::lock_guard<std::mutex> lock(g_cb_mutex);
            if (g_message_cb) {
                g_message_cb(from.c_str(), payload.c_str());
            }
        }
    );
}

} // extern "C"
