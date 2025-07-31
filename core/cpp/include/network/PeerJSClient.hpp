#pragma once
#include <vector>
#include <string>

// Récupère la liste des peers présents sur PeerJS
std::vector<std::string> getPeers(const std::string& host,
                                  int port,
                                  bool secure,
                                  long timeoutS = 5,
                                  const std::string& key = "peerjs");
