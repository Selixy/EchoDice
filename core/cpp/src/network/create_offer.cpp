// core/cpp/src/network/create_offer.cpp
#include "info.hpp"
#include "network/create_offer.hpp"
#include "network/peer_manager.hpp"
#include "network/temp_id.hpp"
#include <rtc/rtc.hpp>
#include <iostream>

namespace network {

void create_offer(PeerManager& manager) {
    // 1) Création de la PeerConnection + ID temporaire
    auto pc = std::make_shared<rtc::PeerConnection>();
    std::string tempId = make_temp_id();

    // 2) DataChannel "meta" pour échanger les IDs
    auto dcMeta = pc->createDataChannel("meta");
    // à l'ouverture, on envoie NOTRE ID
    dcMeta->onOpen([dcMeta] {
        dcMeta->send(gInfo.ID);
    });
    // à la réception, on renomme dans PeerManager
    dcMeta->onMessage([&manager, tempId](auto data) {
        if (auto txt = std::get_if<std::string>(&data)) {
            manager.rename(tempId, *txt);
            std::cout << "[network] renommé tempId '" << tempId
                      << "' → '" << *txt << "'\n";
        }
    });

    // 3) Callback pour afficher l’OFFER
    pc->onLocalDescription([tempId](rtc::Description desc) {
        std::string raw = static_cast<std::string>(desc);
        std::cout << "\n=== OFFER [" << tempId << "] ===\n"
                  << raw << "\n====================\n";
    });

    // 4) stockage sous la clé temporaire
    manager.insert(tempId, pc);

    // 5) ouverture du canal "data" et génération de l’offre
    pc->createDataChannel("data");
    pc->setLocalDescription();
}

} // namespace network
