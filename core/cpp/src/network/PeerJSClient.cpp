#include <nlohmann/json.hpp>
#include "network/PeerJSClient.hpp"
#include <curl/curl.h>
#include <stdexcept>
#include <sstream>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), total);
    return total;
}

std::vector<std::string> getPeers(const std::string& host,
                                  int port,
                                  bool secure,
                                  long timeoutS,
                                  const std::string& key)
{
    std::ostringstream url;
    url << (secure ? "https://" : "http://")
        << host << ":" << port
        << "/peerjs/peers?key=" << key;

    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Impossible d'initialiser CURL");

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutS);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::string err = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error("Requête HTTP échouée: " + err);
    }
    curl_easy_cleanup(curl);

    // Parse la réponse JSON
    auto j = nlohmann::json::parse(response);
    std::vector<std::string> peers;
    for (auto& id : j) {
        peers.push_back(id.get<std::string>());
    }
    return peers;
}
