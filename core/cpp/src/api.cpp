#include <memory>
#include "api.h"
#include "info.hpp"
#include "Logger.hpp"
#include "network/RoomManager.hpp"

static std::unique_ptr<RoomManager> gRoom;
static std::string s_code;

extern "C" {

API_Cpp void Set_ID(const char* id) {
    if (id) gInfo.ID = id;
}

API_Cpp const char* network_GetCode() {
    if (!gRoom) gRoom = std::make_unique<RoomManager>(gInfo.ID);
    s_code = gRoom->createAndJoin();
    LOG_DEBUG("Code room genere = " + s_code);
    return s_code.c_str();
}

API_Cpp void network_ConnectTo(const char* code) {
    if (code) {
        if (!gRoom) gRoom = std::make_unique<RoomManager>(gInfo.ID);
        gRoom->join(std::string(code));
    }
}

API_Cpp bool network_SendMessage(const char* peer_id,
                                 const char* message)
{
    return false;
}

API_Cpp void network_Shutdown() {
    if (gRoom) {
        gRoom->shutdown();
        gRoom.reset();
    }
    s_code.clear();
}

} // extern "C"
