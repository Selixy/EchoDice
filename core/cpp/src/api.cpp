#include "api.h"
#include "info.hpp"
#include "Logger.hpp"
#include "network/p2p/P2PManager.hpp"

static std::string s_code;

extern "C" {

API_Cpp void Set_ID(const char* id) {
    if (id) gInfo.ID = id;
}

API_Cpp const char* network_GetCode() {
    p2p::P2PManager::init(gInfo.ID);
    s_code = p2p::P2PManager::getRoomCode();
    LOG_DEBUG("PeerJS connecté, code généré = " + s_code);
    return s_code.c_str();
}

API_Cpp void network_ConnectTo(const char* code) {
    if (code) {
        p2p::P2PManager::init(gInfo.ID);
        p2p::P2PManager::joinRoom(code);
    }
}

API_Cpp bool network_SendMessage(const char* peer_id,
                                 const char* message)
{
    if (!peer_id || !message) return false;
    p2p::P2PManager::send(peer_id, message);
    return true;
}

API_Cpp void network_Shutdown() {
    p2p::P2PManager::shutdown();
}

} // extern "C"
