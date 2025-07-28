#include "network/PeerConnector.hpp"
#include "network/SignalCodec.hpp"

#include <future>
#include <memory>

namespace signaling {

std::string connectToPeer(network::PeerManager& pm,
                          const std::string& myId,
                          const std::string& remoteCode)
{
    auto j = decode(remoteCode);
    auto remoteSdp = j.at("sdp").get<std::string>();

    auto promisePtr = std::make_shared<std::promise<std::string>>();
    auto futureAns  = promisePtr->get_future();

    pm.onLocalDescription(
        [promisePtr, myId](rtc::Description desc) {
            auto answerSdp = std::string(desc);
            auto answerB64 = encode(myId, answerSdp);
            promisePtr->set_value(answerB64);
        }
    );

    pm.SetRemoteDescription("offer", remoteSdp);
    pm.CreateAnswer();

    return futureAns.get();
}

void setupMessageHandler(network::PeerManager& pm,
                         std::function<void(const std::string&)> onConnected)
{
    pm.onMessage(
        [&pm, onConnected](const std::string& from,
                           const std::string& payload) {
            auto j   = decode(payload);
            auto sdp = j.at("sdp").get<std::string>();
            pm.SetRemoteDescription("answer", sdp);
            onConnected(from);
        }
    );
}

} // namespace signaling
