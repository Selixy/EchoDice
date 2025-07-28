#include "network/PeerConnector.hpp"
#include "network/SignalCodec.hpp"

#include <future>
#include <memory>
#include <iostream>

namespace signaling {

std::string connectToPeer(network::PeerManager& pm,
                          const std::string& myId,
                          const std::string& remoteCode)
{
    std::cout << "[PeerConnector] connectToPeer called\n";

    // 1) Décodage {id, sdp}
    auto j = decode(remoteCode);
    auto remoteId  = j.at("id").get<std::string>();
    auto remoteSdp = j.at("sdp").get<std::string>();
    std::cout << "[PeerConnector] remoteId = " << remoteId << "\n";
    std::cout << "[PeerConnector] remoteSdp =\n" << remoteSdp << "\n";

    // 2) Prépare la promise pour récupérer l’ANSWER
    auto promisePtr = std::make_shared<std::promise<std::string>>();
    auto futureAns  = promisePtr->get_future();

    pm.onLocalDescription(
        [promisePtr, myId](rtc::Description desc) {
            std::cout << "[PeerConnector] onLocalDescription (ANSWER) received\n";
            auto answerSdp = std::string(desc);
            std::cout << "[PeerConnector] answerSdp =\n" << answerSdp << "\n";
            auto answerB64 = encode(myId, answerSdp);
            std::cout << "[PeerConnector] encoded ANSWER = " << answerB64 << "\n";
            promisePtr->set_value(answerB64);
        }
    );

    // 3) Applique l’offre distante puis crée l’ANSWER
    std::cout << "[PeerConnector] Applying remote offer...\n";
    pm.SetRemoteDescription("offer", remoteSdp);
    std::cout << "[PeerConnector] Creating answer...\n";
    pm.CreateAnswer();

    // 4) Renvoie la chaîne Base64
    auto result = futureAns.get();
    std::cout << "[PeerConnector] connectToPeer returning answerBase64\n";
    return result;
}

void setupMessageHandler(network::PeerManager& pm,
                         std::function<void(const std::string&)> onConnected)
{
    std::cout << "[PeerConnector] setupMessageHandler called\n";
    pm.onMessage(
        [&pm, onConnected](const std::string& from,
                           const std::string& payload) {
            std::cout << "[PeerConnector] onMessage from " << from << "\n";
            auto j   = decode(payload);
            auto sdp = j.at("sdp").get<std::string>();
            std::cout << "[PeerConnector] received final answer SDP:\n" << sdp << "\n";
            pm.SetRemoteDescription("answer", sdp);
            std::cout << "[PeerConnector] Applied final answer SDP, invoking onConnected\n";
            onConnected(from);
        }
    );
}

} // namespace signaling
