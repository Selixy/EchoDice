#include "network/OfferManager.hpp"
#include "network/SignalCodec.hpp"

#include <future>
#include <memory>

namespace signaling {

std::string getOfferCode(network::PeerManager& pm,
                         const std::string& myId)
{
    // promise partagée pour être copyable dans la lambda
    auto promisePtr = std::make_shared<std::promise<std::string>>();
    auto futureSDP  = promisePtr->get_future();

    pm.onLocalDescription(
        [promisePtr](rtc::Description desc) {
            promisePtr->set_value(std::string(desc));
        }
    );

    pm.CreateOffer();

    auto sdp = futureSDP.get();
    return encode(myId, sdp);
}

} // namespace signaling
