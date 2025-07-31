#pragma once
#include <map>
#include <set>
#include <string>
#include <functional>
#include <memory>
#include <rtc/rtc.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using DCOpenCb    = std::function<void(const std::string& peerId,
                                       std::shared_ptr<rtc::DataChannel>)>;
using DCMessageCb = std::function<void(const std::string& peerId,
                                       const std::string& msg)>;

class PeerManager {
public:
  PeerManager(std::function<void(const std::string& type,
                                 const std::string& dst,
                                 const std::string& payload)> sendSignaling);

  void connectToPeer(const std::string& peerId);
  void handleOffer(const std::string& peerId, const std::string& sdp);
  void handleAnswer(const std::string& peerId, const std::string& sdp);
  void handleIce(const std::string& peerId,
                 const std::string& candidate, 
                 const std::string& mid);

  void onDataChannelOpen(DCOpenCb cb)    { dcOpenCb_    = std::move(cb); }
  void onDataChannelMessage(DCMessageCb cb) { dcMessageCb_ = std::move(cb); }

private:
  struct Conn {
    std::shared_ptr<rtc::PeerConnection> pc;
    std::shared_ptr<rtc::DataChannel>    dc;
  };
  std::map<std::string, Conn>                        conns_;
  std::set<std::string>                              pendingOffer_;
  std::function<void(const std::string&, const std::string&, const std::string&)> send_;
  DCOpenCb     dcOpenCb_;
  DCMessageCb  dcMessageCb_;

  void setupPC(const std::string& peerId);
};
