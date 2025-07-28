// core/cpp/src/network/create_offer.cpp

#include "info.hpp"
#include "network/create_offer.hpp"
#include "network/peer_manager.hpp"
#include "network/temp_id.hpp"
#include "network/sdp_codec.hpp"
#include <rtc/rtc.hpp>
#include <iostream>

namespace network {

void create_offer(PeerManager& manager) {
    // 0) DEBUG : on rentre bien dans la fonction
    const std::string id = gInfo.ID;
    std::cout << "[DEBUG] create_offer() invoked for ID=" << id << std::endl;

    // 1) Création de la PeerConnection + ID temporaire
    auto pc = std::make_shared<rtc::PeerConnection>();
    std::string tempId = make_temp_id();

    // 2) Hook sur tout DataChannel entrant (pour le "meta")
    pc->onDataChannel([&manager, tempId](auto incoming) {
        if (incoming->label() == "meta") {
            // à l'ouverture, on envoie notre ID
            incoming->onOpen([incoming] {
                incoming->send(gInfo.ID);
            });
            // à la réception, on renomme la connexion
            incoming->onMessage([&manager, tempId](auto data) {
                if (auto txt = std::get_if<std::string>(&data)) {
                    manager.rename(tempId, *txt);
                    std::cout << "[network] renommé tempId '"
                              << tempId << "' → '" << *txt << "'\n";
                }
            });
        }
    });

    // 3) Callback : dès que la SDP locale est prête
    pc->onLocalDescription([tempId](rtc::Description desc) {
        std::string raw = static_cast<std::string>(desc);
        // 3.a) RAW SDP
        std::cout << "[DEBUG RAW OFFER][" << tempId << "]\n"
                  << raw << std::endl;
        // 3.b) OFFER encodée
        std::string encoded = encode_sdp(raw);
        std::cout << "[OFFER][" << tempId << "] "
                  << encoded << std::endl;
    });

    // 4) DataChannel “data” et stockage
    pc->createDataChannel("data");
    manager.insert(tempId, pc);

    // 5) Création du canal "meta" pour lancer l’échange d’IDs
    pc->createDataChannel("meta");

    // 6) Génére l’offre
    pc->setLocalDescription();
}

} // namespace network
