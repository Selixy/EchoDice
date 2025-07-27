#pragma once
#include <functional>
#include <string>

namespace network {
class PeerManager;

/** Type du callback applicatif (pairId, message) */
using MessageHandler = std::function<void(const std::string&,
                                          const std::string&)>;


bool attach_receiver(PeerManager&      manager,
                     const std::string& peerId,
                     MessageHandler     handler,
                     const std::string& label = "data");
} // namespace network
