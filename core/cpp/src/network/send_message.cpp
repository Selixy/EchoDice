#include "network/send_message.hpp"
#include "network/peer_manager.hpp"
#include <rtc/rtc.hpp>
#include <iostream>

namespace network {

bool send_message(PeerManager& manager,
                  const std::string& peerId,
                  const std::string& message,
                  const std::string& label)
{
    // 1) On récupère la PeerConnection pour ce peer
    auto pc = manager.get(peerId);
    if (!pc) {
        std::cerr << "[network] send_message: unknown peer '" << peerId << "'\n";
        return false;
    }

    // 2) On crée (ou récupère) le DataChannel
    auto dc = pc->createDataChannel(label);   // createDataChannel renvoie l’existant si même label
    if (!dc) {
        std::cerr << "[network] send_message: cannot create DataChannel '"
                  << label << "' for peer '" << peerId << "'\n";
        return false;
    }

    // 3) Si le canal est déjà ouvert → on envoie tout de suite
    if (dc->isOpen()) {
        dc->send(message);
        return true;
    }

    // 4) Sinon on l’enverra dès l’ouverture
    dc->onOpen([weak = std::weak_ptr<rtc::DataChannel>(dc),
                msg  = std::string(message)]() {
        if (auto ch = weak.lock()) ch->send(msg);
    });

    return true;
}

} // namespace network
