#include "api.h"
#include "network/network.hpp"
#include <string>

extern "C" {

API_Cpp void network_Init() {
  network::init();
}

API_Cpp void network_Shutdown() {
  network::shutdown();
}

API_Cpp void network_ConectTo(const char* remote_sdp) {
  if (!remote_sdp) return;
  // applique l’offer
  network::connectTo(std::string(remote_sdp));
  // on a déjà printé l’answer dans connectTo()
}


API_Cpp const char* network_GetLastSdp() {
  const std::string& s = network::lastSdp();
  return s.c_str();
}

} // extern "C"
