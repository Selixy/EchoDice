#pragma once

#include <string>
#include <functional>
#include <memory>
#include <variant>
#include <rtc/rtc.hpp>

namespace network {

/// Wrapping simplifié de rtc::PeerConnection + DataChannel
class PeerManager {
public:
    PeerManager();

    /// Callback quand la desc locale est prête (offer ou answer)
    void onLocalDescription(std::function<void(rtc::Description)> cb);

    /// Crée une OFFER
    void CreateOffer();

    /// Applique un SDP distant (offer ou answer)
    void SetRemoteDescription(const std::string& /*type*/,
                              const std::string& sdp);

    /// Crée une ANSWER
    void CreateAnswer();

    /// Callback message reçu via le DataChannel
    void onMessage(std::function<void(const std::string& from,
                                      const std::string& payload)> cb);

    /// Envoie un message (texte) si le channel est ouvert
    bool SendMessage(const char* peer_id, const char* message);

    /// Envoie dès que le channel sera prêt
    void SendWhenReady(const std::string& message);

    /// Ferme DataChannel + PeerConnection
    void Close();

private:
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel> data_channel;
    std::function<void(const std::string&, const std::string&)> message_cb;
    std::string pending_message_;
};

} // namespace network
