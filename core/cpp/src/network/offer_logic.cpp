#include "network/offer_logic.hpp"
#include <rtc/rtc.hpp>
#include <memory>
#include <mutex>
#include <cstdio>

namespace {
    static std::shared_ptr<rtc::PeerConnection> pc;
    static std::mutex                           initMtx;

    void ensurePeer() {
        std::lock_guard<std::mutex> lock(initMtx);
        if (pc) return;

        rtc::InitLogger(rtc::LogLevel::Info);
        rtc::Preload();

        rtc::Configuration config;
        config.iceServers.emplace_back("stun:stun.l.google.com:19302");
        pc = std::make_shared<rtc::PeerConnection>(config);

        pc->onLocalDescription([](rtc::Description desc) {
            std::string s = static_cast<std::string>(desc);
            std::printf("\n=== OFFER SDP ===\n%s\n=================\n", s.c_str());
        });


        pc->createDataChannel("data");
    }
}

namespace network {

void createOffer() {
    ensurePeer();
    pc->setLocalDescription();
}

}
