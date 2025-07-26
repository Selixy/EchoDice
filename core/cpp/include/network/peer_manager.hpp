#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <rtc/rtc.hpp>

namespace network {

class PeerManager {
public:
    void insert(const std::string& id, std::shared_ptr<rtc::PeerConnection> peer);
    std::shared_ptr<rtc::PeerConnection> get(const std::string& id);
    void remove(const std::string& id);
    void clear();

private:
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peers_;
    std::mutex mtx_;
};

} // namespace network
