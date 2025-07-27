#include "network/message_dispatcher.hpp"
#include "network/peer_manager.hpp"
#include <rtc/rtc.hpp>
#include <iostream>

namespace {

void hook_channel(const std::shared_ptr<rtc::DataChannel>& dc,
                  const std::string&                       peerId,
                  network::MessageHandler                  handler)
{
    dc->onMessage([peerId, handler](auto data) {
        if (auto pTxt = std::get_if<std::string>(&data))
            handler(peerId, *pTxt);
    });
}

} // unnamed

namespace network {

bool attach_receiver(PeerManager&       manager,
                     const std::string& peerId,
                     MessageHandler     handler,
                     const std::string& label)
{
    auto pc = manager.get(peerId);
    if (!pc) {
        std::cerr << "[network] attach_receiver: unknown peer '" << peerId << "'\n";
        return false;
    }

    /* 1) Cas d’un DataChannel déjà existant (même label) */
    auto dc = pc->createDataChannel(label);
    hook_channel(dc, peerId, std::move(handler));

    /* 2) Cas où le pair en crée un nouveau plus tard */
    pc->onDataChannel([peerId, handler](auto incoming) {
        hook_channel(incoming, peerId, handler);
    });
    return true;
}

} // namespace network
