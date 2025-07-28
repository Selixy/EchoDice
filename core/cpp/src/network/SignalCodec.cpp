#include "network/SignalCodec.hpp"
#include "network/base64.hpp"
using nlohmann::json;

namespace signaling {

std::string encode(const std::string& id, const std::string& sdp) {
    json j;
    j["id"]  = id;
    j["sdp"] = sdp;
    return base64_encode(j.dump());
}

json decode(const std::string& base64) {
    auto raw = base64_decode(base64);
    return json::parse(raw);
}

} // namespace signaling
