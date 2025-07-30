#pragma once
#include <string>

namespace PeerJS {

/// Génère un peerId via la requête HTTP GET /peerjs/id?key=peerjs  
/// – host : hôte (ex. "0.peerjs.com")  
/// – port : port (443)  
/// – secure : true→https, false→http  
/// – timeoutS : timeout HTTP  
std::string generatePeerId(const std::string& host,
                           int port,
                           bool secure,
                           long timeoutS = 10);

} // namespace PeerJS
