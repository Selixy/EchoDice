#include "network/network.hpp"
#include <mutex>
#include "rtc/rtc.hpp"

namespace network {
static std::shared_ptr<rtc::PeerConnection> pc;
static std::string                           g_lastSdp;
static std::mutex                            g_mutex;

void init() {
  rtc::InitLogger(rtc::LogLevel::Info);
  rtc::Preload();

  rtc::Configuration config;
  config.iceServers.emplace_back("stun:stun.l.google.com:19302");
  pc = std::make_shared<rtc::PeerConnection>(config);

  pc->onLocalDescription([](rtc::Description desc) {
    std::lock_guard<std::mutex> lock(g_mutex);
    // <-- conversion correcte
    g_lastSdp = static_cast<std::string>(desc);
    printf("\n=== OFFER SDP ===\n%s\n=================\n", g_lastSdp.c_str());
  });

  auto dc = pc->createDataChannel("data");
  dc->onOpen([] { printf("[DataChannel] ouvert ✅\n"); });

  pc->setLocalDescription();
}

void shutdown() {
  if (pc) {
    pc->close();
    pc.reset();
  }
}

const std::string& connectTo(const std::string& remoteSdp) {
  if (!pc) throw std::runtime_error("PeerConnection non initialisée");

  pc->setRemoteDescription(rtc::Description(remoteSdp));

  pc->onLocalDescription([](rtc::Description desc) {
    std::lock_guard<std::mutex> lock(g_mutex);
    // <-- conversion correcte
    g_lastSdp = static_cast<std::string>(desc);
    printf("\n=== ANSWER SDP ===\n%s\n==================\n", g_lastSdp.c_str());
  });

  pc->setLocalDescription();

  std::lock_guard<std::mutex> lock(g_mutex);
  return g_lastSdp;
}

const std::string& lastSdp() {
  std::lock_guard<std::mutex> lock(g_mutex);
  return g_lastSdp;
}
} // namespace network
