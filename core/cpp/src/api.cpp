#include "api.h"
#include "info.hpp"

#include "network/PeerJSClient.hpp"

#include <nlohmann/json.hpp>
#include <iostream>



using nlohmann::json;


extern "C" {

API_Cpp void network_GetCode() {
    try {
        if (gInfo.ID.empty()) {
            std::cerr << "[api:GetCode] ID not set. Call Set_ID() first.\n";
            return;
        }
        auto peerId = PeerJS::generatePeerId();
        gInfo.ID = peerId;
        std::cout << "[PeerJS ID] " << peerId << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[api:GetCode] exception: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[api:GetCode] unknown exception\n";
    }
}

API_Cpp void Set_ID(const char* id) {
    try {
        if (id) {
            gInfo.ID = id;
        }
    } catch (const std::exception& e) {
        std::cerr << "[api:Set_ID] exception: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[api:Set_ID] unknown exception\n";
    }
}

API_Cpp void network_ConectTo(const char* remote_sdp) {

}

API_Cpp bool network_SendMessage(const char* peer_id,
                                 const char* message)
{

    return false;
}

API_Cpp void network_Shutdown() {

}

} // extern "C"
