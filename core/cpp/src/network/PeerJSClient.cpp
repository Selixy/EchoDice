// PeerJSClient.cpp
#include "network/PeerJSClient.hpp"

#include <curl/curl.h>
#include <stdexcept>
#include <sstream>

namespace {

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), total);
    return total;
}

} // anonymous

namespace PeerJS {

std::string generatePeerId(
    const std::string& apiKey,
    const std::string& host,
    int                 port,
    bool                secure,
    long                timeoutS
) {
    // Construire l'URL
    std::ostringstream oss;
    oss << (secure ? "https://" : "http://")
        << host << ":"
        << port
        << "/peerjs/id?key=" << apiKey;
    std::string url = oss.str();

    // Initialiser CURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("PeerJSClient: impossible d'initialiser libcurl");
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL,            url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,      &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,        timeoutS);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::string err = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error("PeerJSClient: requête HTTP échouée: " + err);
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    if (http_code != 200) {
        throw std::runtime_error("PeerJSClient: HTTP " + std::to_string(http_code));
    }

    // Supprimer un éventuel \n final
    if (!response.empty() && (response.back() == '\n' || response.back() == '\r')) {
        response.pop_back();
    }

    return response;
}

} // namespace PeerJS
