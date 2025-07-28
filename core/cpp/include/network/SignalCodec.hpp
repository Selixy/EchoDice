#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace signaling {

/// Sérialise { id, sdp } → JSON → Base64
std::string encode(const std::string& id, const std::string& sdp);

/// Décodage Base64 → JSON
nlohmann::json decode(const std::string& base64);

} // namespace signaling
