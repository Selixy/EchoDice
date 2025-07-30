#pragma once
#include <string>

namespace p2p {

void initLocal(const std::string& localId);
std::string getCode();
void connectTo(const std::string& code);
void sendMessage(const std::string& peerId, const std::string& message);
void shutdown();

} // namespace p2p
