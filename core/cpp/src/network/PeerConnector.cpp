#include "network/PeerConnector.hpp"
#include "network/SignalCodec.hpp"

#include <future>
#include <memory>

namespace signaling {

std::string connectToPeer(network::PeerManager& pm,
                          const std::string& myId,
                          const std::string& remoteCode)
{
    // 1) Décodage {id,sdp}→JSON
    auto j         = decode(remoteCode);
    auto remoteSdp = j.at("sdp").get<std::string>();

    // 2) Promise partagée pour récupérer l’ANSWER
    auto promisePtr = std::make_shared<std::promise<std::string>>();
    auto futureAns  = promisePtr->get_future();

    pm.onLocalDescription(
        [promisePtr, myId](rtc::Description desc) {
            // Description → string, puis encode
            auto answerSdp = std::string(desc);
            auto answerB64 = encode(myId, answerSdp);
            promisePtr->set_value(answerB64);
        }
    );

    // 3) Applique l’offre distante puis crée l’answer
    pm.SetRemoteDescription("offer", remoteSdp);
    pm.CreateAnswer();

    // 4) Récupère et renvoie le Base64 de l’ANSWER
    return futureAns.get();
}

void setupMessageHandler(network::PeerManager& pm,
                         std::function<void(const std::string&)> onConnected)
{
    pm.onMessage(
        // capture pm par référence et onConnected par copie
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
