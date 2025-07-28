#include "network/OfferManager.hpp"
#include "network/SignalCodec.hpp"

#include <future>
#include <memory>

namespace signaling {

std::string getOfferCode(network::PeerManager& pm,
                         const std::string& myId)
{
    // Utilise un shared_ptr pour rendre la lambda copyable
    auto promisePtr = std::make_shared<std::promise<std::string>>();
    auto futureSDP  = promisePtr->get_future();

    pm.onLocalDescription(
        [promisePtr](rtc::Description desc) {
            // convertit desc → std::string
            promisePtr->set_value(std::string(desc));
        }
    );

    pm.CreateOffer();

    // attend la SDP locale
    auto sdp = futureSDP.get();

    // sérialise + encode
    return encode(myId, sdp);
}

} // namespace signaling
