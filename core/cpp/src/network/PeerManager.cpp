#include "network/PeerManager.hpp"

namespace network {

PeerManager::PeerManager() {
    // 1) Crée la PeerConnection avec configuration par défaut
    rtc::Configuration config;
    pc = std::make_shared<rtc::PeerConnection>(config);

    // 2) Si le pair distant crée un DataChannel, on le récupère ici
    pc->onDataChannel([this](std::shared_ptr<rtc::DataChannel> dc) {
        data_channel = dc;
        // Attention : c'est onMessage (et non onMessageString)
        dc->onMessage([this](std::variant<rtc::binary, rtc::string> msg) {
            if (std::holds_alternative<rtc::string>(msg)) {
                if (message_cb) message_cb("", std::get<rtc::string>(msg));
            }
        });
    });
}

void PeerManager::onLocalDescription(std::function<void(rtc::Description)> cb) {
    pc->onLocalDescription(std::move(cb));
}

void PeerManager::CreateOffer() {
    // Crée un DataChannel pour la signalisation
    data_channel = pc->createDataChannel("data");
    data_channel->onMessage([this](std::variant<rtc::binary, rtc::string> msg) {
        if (std::holds_alternative<rtc::string>(msg)) {
            if (message_cb) message_cb("", std::get<rtc::string>(msg));
        }
    });
    // Lance l’offre ; la callback onLocalDescription sera déclenchée automatiquement
    pc->createOffer();
}

void PeerManager::SetRemoteDescription(const std::string& /*type*/,
                                       const std::string& sdp) {
    // La wrapper C++ accepte directement rtc::Description(sdp)
    pc->setRemoteDescription(rtc::Description(sdp));
}

void PeerManager::CreateAnswer() {
    // Lance la réponse ; onLocalDescription sera appelée ensuite
    pc->createAnswer();
}

void PeerManager::onMessage(std::function<void(const std::string&,
                                              const std::string&)> cb) {
    message_cb = std::move(cb);
    // Si un DataChannel existe déjà, rattache immédiatement le handler
    if (data_channel) {
        data_channel->onMessage([this](std::variant<rtc::binary, rtc::string> msg) {
            if (std::holds_alternative<rtc::string>(msg)) {
                if (message_cb) message_cb("", std::get<rtc::string>(msg));
            }
        });
    }
}

bool PeerManager::SendMessage(const char* /*peer_id*/,
                              const char* message) {
    if (!data_channel) return false;
    // Envoie texte
    data_channel->send(std::string(message));
    return true;
}

void PeerManager::Close() {
    if (data_channel) data_channel->close();
    pc->close();
}

} // namespace network
