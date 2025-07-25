#include "network/network.hpp"
#include <iostream>
#include <chrono>

namespace network {

static bool s_initialized = false;

void init() {
    if (s_initialized) return;
    s_initialized = true;
    std::cout << "[network] init()" << std::endl;
    // TODO : setup ice, create PeerConnection, etc.
}

void tick() {
    static int count = 0;
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    std::cout
      << "[network] tick #" << ++count
      << " at " << ms << " ms" << std::endl;
    // TODO : network polling, drive WebRTC, timers…
}

void shutdown() {
    if (!s_initialized) return;
    s_initialized = false;
    std::cout << "[network] shutdown()" << std::endl;
    // TODO : cleanup PeerConnection, logs, etc.
}

} // namespace network
