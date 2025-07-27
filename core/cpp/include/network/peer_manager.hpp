#pragma once
#include <rtc/rtc.hpp>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>

namespace network {

class PeerManager {
public:
    void insert(const std::string& id,
                std::shared_ptr<rtc::PeerConnection> pc);

    std::shared_ptr<rtc::PeerConnection> get(const std::string& id);
    void remove(const std::string& id);
    void clear();
    void rename(const std::string& oldId, const std::string& newId);
    
private:
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peers_;
    std::mutex mtx_;
};

} // namespace network
