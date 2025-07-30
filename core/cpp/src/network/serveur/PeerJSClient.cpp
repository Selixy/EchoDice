#include "network/serveur/PeerJSClient.hpp"
#include <curl/curl.h>
#include <stdexcept>
#include <sstream>

namespace {
static size_t WriteCallback(void* contents,
                            size_t size,
                            size_t nmemb,
                            void* userp)
{
    size_t total = size * nmemb;
    static_cast<std::string*>(userp)
      ->append(static_cast<char*>(contents), total);
    return total;
}
} // anonymous

namespace PeerJS {

std::string generatePeerId(const std::string& host,
                           int port,
                           bool secure,
                           long timeoutS)
{
    std::ostringstream url;
    url << (secure ? "https://" : "http://")
        << host << ":" << port
        << "/peerjs/id?key=peerjs";

    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Impossible d'initialiser CURL");

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL,           url.str().c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,     &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,       timeoutS);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::string err = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error("Requête HTTP échouée: " + err);
    }

    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    curl_easy_cleanup(curl);

    if (code != 200)
        throw std::runtime_error("HTTP " + std::to_string(code));

    // Supprime \n ou \r final si présent
    if (!response.empty() &&
       (response.back() == '\n' || response.back() == '\r'))
        response.pop_back();

    return response;
}

} // namespace PeerJS
