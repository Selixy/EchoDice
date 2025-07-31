#include "network/PeerManager.hpp"

// Constructeur
PeerManager::PeerManager(decltype(send_) sendSignaling)
  : send_(std::move(sendSignaling))
{}

// Instancie PeerConnection + callbacks
void PeerManager::setupPC(const std::string& peerId) {
  if (conns_.count(peerId)) return;

  rtc::Configuration config;
  config.iceServers.emplace_back("stun:stun.l.google.com:19302");

  auto pc = std::make_shared<rtc::PeerConnection>(config);
  conns_[peerId].pc = pc;

  // 1) Descriptions (offer & answer)
  pc->onLocalDescription([this,peerId](rtc::Description desc) {
    std::string sdp = std::string(desc);  // conversion explicite :contentReference[oaicite:3]{index=3}
    json j; j["sdp"] = sdp;

    if (pendingOffer_.erase(peerId)) {
      send_("SDP_OFFER", peerId, j.dump());
    } else {
      send_("SDP_ANSWER", peerId, j.dump());
    }
  });

  // 2) ICE candidates locaux
  pc->onLocalCandidate([this,peerId](rtc::Candidate candidate) {
    json j;
    j["candidate"] = candidate.candidate();  // méthode candidate() :contentReference[oaicite:4]{index=4}
    j["mid"]       = candidate.mid();
    send_("ICE_CANDIDATE", peerId, j.dump());
  });

  // 3) Création DataChannel côté answerer
  pc->onDataChannel([this,peerId](std::shared_ptr<rtc::DataChannel> dc) {
    conns_[peerId].dc = dc;
    dc->onOpen([this,peerId,dc]() {
      if (dcOpenCb_) dcOpenCb_(peerId, dc);
    });
    dc->onMessage([this,peerId](rtc::message_variant v) {
      if (std::holds_alternative<rtc::string>(v) && dcMessageCb_) {
        dcMessageCb_(peerId, std::get<rtc::string>(v));
      }
    });
  });
}

// initie le handshake
void PeerManager::connectToPeer(const std::string& peerId) {
  setupPC(peerId);
  pendingOffer_.insert(peerId);    // marque que le prochain SDP est un OFFER
  conns_[peerId].pc->createOffer(); // déclenche onLocalDescription
}

// Reçoit une offre → set+answer
void PeerManager::handleOffer(const std::string& peerId,
                              const std::string& sdp) {
  setupPC(peerId);
  auto pc = conns_[peerId].pc;
  pc->setRemoteDescription(rtc::Description(sdp));
  pc->createAnswer();              // déclenche onLocalDescription (ANSWER)
}

// Reçoit une answer → setRemote
void PeerManager::handleAnswer(const std::string& peerId,
                               const std::string& sdp) {
  auto it = conns_.find(peerId);
  if (it == conns_.end()) return;
  it->second.pc->setRemoteDescription(rtc::Description(sdp));
}

// Reçoit un ICE → addRemoteCandidate
void PeerManager::handleIce(const std::string& peerId,
                            const std::string& candidate,
                            const std::string& mid) {
  auto it = conns_.find(peerId);
  if (it == conns_.end()) return;
  // ajout du candidat distant :contentReference[oaicite:5]{index=5}
  it->second.pc->addRemoteCandidate(rtc::Candidate(candidate, mid));
}
