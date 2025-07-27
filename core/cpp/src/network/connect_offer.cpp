#include "info.hpp"
#include "network/connect_offer.hpp"
#include "network/peer_manager.hpp"
#include <rtc/rtc.hpp>
#include <iostream>

namespace network {

void connect_to_offer(PeerManager& manager, const std::string& id, const std::string& remoteSdp) {
    auto pc = std::make_shared<rtc::PeerConnection>();
    const std::string id = gInfo.ID;

    pc->onLocalDescription([id](rtc::Description desc) {
        std::cout << "\n=== ANSWER [" << id << "] ===\n"
                  << std::string(desc) << "\n====================\n";
    });

    manager.insert(id, pc);

    pc->setRemoteDescription(rtc::Description(remoteSdp));
    pc->setLocalDescription(); // génère l'answer
}

} // namespace network
