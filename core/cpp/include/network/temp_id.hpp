#pragma once
#include <atomic>
#include <string>

/**
 * @brief Génère un identifiant temporaire unique pour une nouvelle connexion.
 */
inline std::string make_temp_id() {
    static std::atomic<uint64_t> counter{0};
    return "temp-" + std::to_string(++counter);
}
