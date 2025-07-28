#include "network/PeerManager.hpp"
#include <iostream>
#include <variant>

namespace network {

PeerManager::PeerManager() {
    rtc::Configuration config;
    pc = std::make_shared<rtc::PeerConnection>(config);

    std::cout << "[PeerManager] constructed, registering onDataChannel\n";

    pc->onDataChannel([this](std::shared_ptr<rtc::DataChannel> dc) {
        std::cout << "[PeerManager] onDataChannel fired\n";
        data_channel = dc;

        // Réception des messages
        dc->onMessage([this](std::variant<rtc::binary, rtc::string> msg) {
            if (std::holds_alternative<rtc::string>(msg) && message_cb) {
                auto s = std::get<rtc::string>(msg);
                std::cout << "[PeerManager] Received message: " << s << "\n";
                message_cb("", s);
            }
        });

        // Ouverture du canal
        dc->onOpen([this]() {
            std::cout << "[PeerManager] DataChannel is open\n";
            if (!pending_message_.empty()) {
                std::cout << "[PeerManager] Sending pending: " 
                          << pending_message_ << "\n";
                data_channel->send(pending_message_);
                pending_message_.clear();
            }
        });
    });
}

void PeerManager::onLocalDescription(std::function<void(rtc::Description)> cb) {
    std::cout << "[PeerManager] registering onLocalDescription handler\n";
    // On se contente de forwarder l'événement, la local description
    // a déjà été appliquée par libdatachannel.
    pc->onLocalDescription(std::move(cb));
}

void PeerManager::CreateOffer() {
    std::cout << "[PeerManager] CreateOffer: creating DataChannel & offer\n";
    // Création du DataChannel
    data_channel = pc->createDataChannel("data");
    data_channel->onMessage([this](std::variant<rtc::binary, rtc::string> msg) {
        if (std::holds_alternative<rtc::string>(msg) && message_cb) {
            auto s = std::get<rtc::string>(msg);
            std::cout << "[PeerManager] Received message on offerer: " << s << "\n";
            message_cb("", s);
        }
    });
    // Lance la génération de l’offre (libdatachannel l’applique et
    // appellera votre onLocalDescription ensuite)
    pc->createOffer();
}

void PeerManager::SetRemoteDescription(const std::string& /*type*/,
                                       const std::string& sdp) {
    std::cout << "[PeerManager] Setting remote description:\n" << sdp << "\n";
    pc->setRemoteDescription(rtc::Description(sdp));
}

void PeerManager::CreateAnswer() {
    std::cout << "[PeerManager] CreateAnswer: generating answer\n";
    // Lance la génération de l’answer (libdatachannel l’applique et
    // appellera votre onLocalDescription ensuite)
    pc->createAnswer();
}

void PeerManager::onMessage(std::function<void(const std::string&,
                                              const std::string&)> cb) {
    std::cout << "[PeerManager] registering onMessage handler\n";
    message_cb = std::move(cb);
    if (data_channel) {
        std::cout << "[PeerManager] DataChannel exists, reattaching onMessage\n";
        data_channel->onMessage([this](std::variant<rtc::binary, rtc::string> msg){
            if (std::holds_alternative<rtc::string>(msg) && message_cb) {
                auto s = std::get<rtc::string>(msg);
                std::cout << "[PeerManager] Received message on reattached channel: "
                          << s << "\n";
                message_cb("", s);
            }
        });
    }
}

bool PeerManager::SendMessage(const char* /*peer_id*/, const char* message) {
    std::cout << "[PeerManager] SendMessage called with: " << message << "\n";
    if (!data_channel) {
        std::cout << "[PeerManager] No DataChannel → cannot send\n";
        return false;
    }
    if (!data_channel->isOpen()) {
        std::cout << "[PeerManager] DataChannel not open → cannot send\n";
        return false;
    }
    data_channel->send(std::string(message));
    std::cout << "[PeerManager] Message sent successfully\n";
    return true;
}

void PeerManager::SendWhenReady(const std::string& message) {
    std::cout << "[PeerManager] SendWhenReady with: " << message << "\n";
    if (data_channel && data_channel->isOpen()) {
        std::cout << "[PeerManager] channel open → sending now\n";
        data_channel->send(message);
        pending_message_.clear();
    } else {
        std::cout << "[PeerManager] channel not open → storing pending\n";
        pending_message_ = message;
    }
}

void PeerManager::Close() {
    std::cout << "[PeerManager] Close called\n";
    if (data_channel) {
        std::cout << "[PeerManager] closing DataChannel\n";
        data_channel->close();
    }
    std::cout << "[PeerManager] closing PeerConnection\n";
    pc->close();
}

} // namespace network
