#include "api.h"
#include "network/network.hpp"

extern "C" {

API_Cpp void network_Init() {
    network::init();
}

API_Cpp void network_Update() {
    network::tick();
}

API_Cpp void network_Shutdown() {
    network::shutdown();
}

} // extern "C"
