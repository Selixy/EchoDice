#pragma once

#include <string>
#include <functional>
#include <memory>
#include <variant>
#include <rtc/rtc.hpp>

namespace network {

/// Gestion simplifiée d’un rtc::PeerConnection + DataChannel pour la signalisation.
class PeerManager {
public:
    PeerManager();

    /// Callback appelé quand la description locale (offer/answer) est prête.
    void onLocalDescription(std::function<void(rtc::Description)> cb);

    /// Lance la création d’une OFFER.
    void CreateOffer();

    /// Applique un SDP distant (offer ou answer).
    void SetRemoteDescription(const std::string& /*type*/,
                              const std::string& sdp);

    /// Lance la création d’une ANSWER.
    void CreateAnswer();

    /// Callback sur message texte reçu (via DataChannel).
    void onMessage(std::function<void(const std::string& from,
                                      const std::string& payload)> cb);

    /// Envoie un message texte sur le DataChannel (ignore peer_id).
    bool SendMessage(const char* peer_id, const char* message);

    /// Ferme proprement DataChannel + PeerConnection.
    void Close();

private:
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> data_channel;
    std::function<void(const std::string&, const std::string&)> message_cb;
};

} // namespace network
