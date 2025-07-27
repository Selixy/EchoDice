#include "info.hpp"
#include "network/create_offer.hpp"
#include "network/peer_manager.hpp"
#include "network/sdp_codec.hpp" // <<== ajout
#include <rtc/rtc.hpp>
#include <iostream>

namespace network {

void create_offer(PeerManager& manager, const std::string& id) {
    auto pc = std::make_shared<rtc::PeerConnection>();
    const std::string id = gInfo.ID;

    pc->onLocalDescription([id](rtc::Description desc) {
        std::string raw = static_cast<std::string>(desc);
        std::cerr << "[DEBUG RAW OFFER] \n" << raw << "\n";
        std::string encoded = encode_sdp(raw);
        std::cout << "\n=== OFFER [" << id << "] ===\n"
                << encoded << "\n====================\n";
    });


    pc->createDataChannel("data");
    manager.insert(id, pc);

    pc->setLocalDescription();
}

}
