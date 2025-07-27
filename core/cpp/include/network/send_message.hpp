#pragma once
#include <string>

namespace network {
class PeerManager;


bool send_message(PeerManager&        manager,
                  const std::string&  peerId,
                  const std::string&  message,
                  const std::string&  label = "data");
} // namespace network
