// PeerJSClient.hpp
#ifndef PEERJSCLIENT_HPP
#define PEERJSCLIENT_HPP

#include <string>

namespace PeerJS {


std::string generatePeerId(
    const std::string& apiKey   = "peerjs",
    const std::string& host     = "0.peerjs.com",
    int                 port     = 443,
    bool                secure   = true,
    long                timeoutS = 5
);

}

#endif
