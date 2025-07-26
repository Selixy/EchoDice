#pragma once
#include <string>

namespace network {

/// Encode une string (SDP) en version compressée/copiable
std::string encode_sdp(const std::string& sdp);

/// Décode un string compressé en SDP original
std::string decode_sdp(const std::string& encoded);

}
