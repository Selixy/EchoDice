#include "info.hpp"
#include "network/connect_offer.hpp"
#include "network/peer_manager.hpp"
#include "network/sdp_codec.hpp"
#include <rtc/rtc.hpp>
#include <iostream>

namespace network {

void connect_to_offer(PeerManager& manager,
                      const std::string& encodedOffer)
{
    // 1) Création de la PeerConnection
    auto pc = std::make_shared<rtc::PeerConnection>();

    // 2) Identifiant utilisé comme clé (et dans les logs)
    const std::string id = gInfo.ID;   // ← variable unique : « id »

    // 3) Callback : affichage de l’ANSWER
    pc->onLocalDescription([id](rtc::Description desc) {
        std::cout << "\n=== ANSWER for [" << id << "] ===\n"
                  << std::string(desc) << "\n====================\n";
    });

    // 4) On range la connexion sous cette clé
    manager.insert(id, pc);

    // 5) Décodage de l’offre + application
    std::string plainSdp = decode_sdp(encodedOffer);
    pc->setRemoteDescription(rtc::Description{plainSdp});

    // 6) Génération de l’ANSWER
    pc->setLocalDescription();
}

} // namespace network
