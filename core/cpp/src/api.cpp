#include "api.h"
#include "network/network.hpp"

extern "C" {

API_NETWORK_API void network_Init() {
    network::init();
}

API_NETWORK_API void network_Update() {
    network::tick();
}

API_NETWORK_API void network_Shutdown() {
    network::shutdown();
}

} // extern "C"
