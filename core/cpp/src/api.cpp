#include "api.h"
#include "info.hpp"

#include "network/OfferManager.hpp"
#include "network/PeerConnector.hpp"
#include "network/SignalCodec.hpp"

#include <iostream>
using nlohmann::json;

static network::PeerManager g_peerManager;

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

    // 1) On décode la structure JSON/Base64 pour extraire l'ID du pair distant
    json j = signaling::decode(remote_sdp);
    std::string remoteId = j.at("id").get<std::string>();

    // 2) On crée l'ANSWER encodé
    auto answerCode = signaling::connectToPeer(
        g_peerManager, gInfo.ID, remote_sdp
    );

    // 3) On envoie immédiatement la réponse sur le DataChannel
    bool ok = g_peerManager.SendMessage(remoteId.c_str(), answerCode.c_str());
    if (!ok) {
        std::cerr << "[Error] Failed to send answer to peer " 
                  << remoteId << std::endl;
    }
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

} // extern "C"
