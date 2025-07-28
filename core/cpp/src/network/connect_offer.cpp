// core/cpp/src/network/connect_offer.cpp
#include "info.hpp"
#include "network/connect_offer.hpp"
#include "network/peer_manager.hpp"
#include "network/temp_id.hpp"
#include <rtc/rtc.hpp>
#include <iostream>

namespace network {

void connect_to_offer(PeerManager& manager,
                      const std::string& plainSdp)
{
    // 1) PeerConnection + ID temporaire
    auto pc = std::make_shared<rtc::PeerConnection>();
    std::string tempId = make_temp_id();

    // 2) Hook sur tout DataChannel entrant
    pc->onDataChannel([&manager, tempId](auto incoming) {
        // Si c'est le canal "meta" : échange d'IDs
        if (incoming->label() == "meta") {
            // à l'ouverture, on renvoie NOTRE ID
            incoming->onOpen([incoming] {
                incoming->send(gInfo.ID);
            });
            // à la réception, on renomme la connexion
            incoming->onMessage([&manager, tempId](auto data) {
                if (auto txt = std::get_if<std::string>(&data)) {
                    manager.rename(tempId, *txt);
                    std::cout << "[network] renommé tempId '" << tempId
                              << "' → '" << *txt << "'\n";
                }
            });
        }
    });

    // 3) Callback pour afficher l’ANSWER
    pc->onLocalDescription([tempId](rtc::Description desc) {
        std::cout << "\n=== ANSWER [" << tempId << "] ===\n"
                  << std::string(desc) << "\n====================\n";
    });

    // 4) Stockage sous clé temporaire
    manager.insert(tempId, pc);

    // 5) Applique l’offre reçue et génère la réponse
    pc->setRemoteDescription(rtc::Description{plainSdp});
    pc->setLocalDescription();
}

} // namespace network
