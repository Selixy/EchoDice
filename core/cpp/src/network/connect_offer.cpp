#include "info.hpp"
#include "network/connect_offer.hpp"
#include "network/peer_manager.hpp"
#include <rtc/rtc.hpp>
#include <iostream>

namespace network {

void connect_to_offer(PeerManager& manager,
                      const std::string& plainSdp)   // ← plus encodée
{
    auto pc = std::make_shared<rtc::PeerConnection>();
    const std::string id = gInfo.ID;

    pc->onLocalDescription([id](rtc::Description desc) {
        std::cout << "\n=== ANSWER [" << id << "] ===\n"
                  << std::string(desc) << "\n====================\n";
    });

    manager.insert(id, pc);

    pc->setRemoteDescription(rtc::Description{plainSdp});
    pc->setLocalDescription();
}

} // namespace network
