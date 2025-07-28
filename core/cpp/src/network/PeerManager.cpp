#include "network/PeerManager.hpp"

namespace network {

PeerManager::PeerManager() {
    rtc::Configuration config;
    pc = std::make_shared<rtc::PeerConnection>(config);

    pc->onDataChannel([this](std::shared_ptr<rtc::DataChannel> dc) {
        data_channel = dc;

        // réception de messages
        dc->onMessage([this](std::variant<rtc::binary, rtc::string> msg) {
            if (std::holds_alternative<rtc::string>(msg) && message_cb) {
                message_cb("", std::get<rtc::string>(msg));
            }
        });

        // dès que le channel s’ouvre, envoie le pending_message_
        dc->onOpen([this]() {
            if (!pending_message_.empty()) {
                data_channel->send(pending_message_);
                pending_message_.clear();
            }
        });
    });
}

void PeerManager::onLocalDescription(std::function<void(rtc::Description)> cb) {
    pc->onLocalDescription(std::move(cb));
}

void PeerManager::CreateOffer() {
    data_channel = pc->createDataChannel("data");
    data_channel->onMessage([this](std::variant<rtc::binary, rtc::string> msg) {
        if (std::holds_alternative<rtc::string>(msg) && message_cb) {
            message_cb("", std::get<rtc::string>(msg));
        }
    });
    pc->createOffer();
}

void PeerManager::SetRemoteDescription(const std::string& /*type*/,
                                       const std::string& sdp) {
    pc->setRemoteDescription(rtc::Description(sdp));
}

void PeerManager::CreateAnswer() {
    pc->createAnswer();
}

void PeerManager::onMessage(std::function<void(const std::string&,
                                              const std::string&)> cb) {
    message_cb = std::move(cb);
    if (data_channel) {
        data_channel->onMessage([this](std::variant<rtc::binary, rtc::string> msg){
            if (std::holds_alternative<rtc::string>(msg) && message_cb) {
                message_cb("", std::get<rtc::string>(msg));
            }
        });
    }
}

bool PeerManager::SendMessage(const char* /*peer_id*/,
                              const char* message) {
    if (!data_channel) return false;
    data_channel->send(std::string(message));
    return true;
}

void PeerManager::SendWhenReady(const std::string& message) {
    if (data_channel && data_channel->isOpen()) {
        data_channel->send(message);
        pending_message_.clear();
    } else {
        pending_message_ = message;
    }
}

void PeerManager::Close() {
    if (data_channel) data_channel->close();
    pc->close();
}

} // namespace network
