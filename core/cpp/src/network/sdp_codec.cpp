#include "network/sdp_codec.hpp"
#include <zlib.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

namespace network {

/// Encode un buffer en base64
static std::string base64_encode(const uint8_t* data, size_t len) {
    static const char* table =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    for (size_t i = 0; i < len; i += 3) {
        uint32_t v = data[i] << 16;
        if (i + 1 < len) v |= data[i + 1] << 8;
        if (i + 2 < len) v |= data[i + 2];
        out += table[(v >> 18) & 0x3F];
        out += table[(v >> 12) & 0x3F];
        out += (i + 1 < len) ? table[(v >> 6) & 0x3F] : '=';
        out += (i + 2 < len) ? table[v & 0x3F] : '=';
    }
    return out;
}

/// Decode un string base64 en buffer
static std::vector<uint8_t> base64_decode(const std::string& str) {
    static const char decode_table[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 0x00-0x0F
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 0x10-0x1F
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63, // 0x20-0x2F
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1, 0,-1,-1, // 0x30-0x3F
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14, // 0x40-0x4F
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1, // 0x50-0x5F
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40, // 0x60-0x6F
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1  // 0x70-0x7F
    };
    std::vector<uint8_t> out;
    uint32_t buffer = 0;
    int bits = 0;
    for (char c : str) {
        if (c == '=') break;
        int val = decode_table[static_cast<unsigned char>(c)];
        if (val == -1) continue;
        buffer = (buffer << 6) | val;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((buffer >> bits) & 0xFF);
        }
    }
    return out;
}

std::string encode_sdp(const std::string& sdp) {
    uLongf compressed_size = compressBound(sdp.size());
    std::vector<uint8_t> compressed(compressed_size);
    if (compress(compressed.data(), &compressed_size,
                 reinterpret_cast<const Bytef*>(sdp.data()), sdp.size()) != Z_OK) {
        throw std::runtime_error("compression failed");
    }
    return base64_encode(compressed.data(), compressed_size);
}

std::string decode_sdp(const std::string& encoded) {
    auto compressed = base64_decode(encoded);
    std::vector<char> out(65536); // taille max SDP décompressé
    uLongf out_size = out.size();
    if (uncompress(reinterpret_cast<Bytef*>(out.data()), &out_size,
                   compressed.data(), compressed.size()) != Z_OK) {
        throw std::runtime_error("decompression failed");
    }
    return std::string(out.data(), out_size);
}

} // namespace network
