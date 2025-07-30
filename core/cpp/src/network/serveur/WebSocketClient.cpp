#include "network/serveur/WebSocketClient.hpp"
#include "Logger.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <thread>
#include <atomic>

namespace PeerJS {
namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
namespace ssl       = net::ssl;
using tcp           = net::ip::tcp;

struct WebSocketClient::Impl {
  std::string host, port, target;
  net::io_context    ioc;
  ssl::context       ctx{ssl::context::tlsv12_client};
  websocket::stream<ssl::stream<tcp::socket>> ws;
  tcp::resolver      resolver{ioc};
  std::thread        thr;
  std::atomic<bool>  running{false};
  RawMessageCallback cb;

  Impl(const std::string& h, const std::string& p, const std::string& t)
    : host(h), port(p), target(t),
      ws(ioc, ctx)
  {
    ctx.set_verify_mode(ssl::verify_none);
  }
  ~Impl() { stop(); }

  void connect() {
    LOG_INFO("Resolving " + host + ":" + port);
    auto results = resolver.resolve(host, port);
    LOG_INFO("Connecting TCP");
    net::connect(ws.next_layer().next_layer(), results.begin(), results.end());

    // SNI
    SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host.c_str());
    LOG_INFO("SSL handshake");
    ws.next_layer().handshake(ssl::stream_base::client);
    LOG_INFO("WebSocket handshake");
    ws.handshake(host, target);
    running = true;

    // thread de lecture
    thr = std::thread([&](){
      while (running) {
        beast::flat_buffer buf;
        beast::error_code ec;
        ws.read(buf, ec);
        if (ec) break;
        std::string msg = beast::buffers_to_string(buf.data());
        if (cb) cb(msg);
      }
    });
    ioc.run();
  }

  void send(const std::string& m) {
    beast::error_code ec;
    ws.write(net::buffer(m), ec);
    if (ec) LOG_ERROR("WS send error: " + ec.message());
  }

  void stop() {
    if (!running) return;
    running = false;
    beast::error_code ec;
    ws.close(websocket::close_code::normal, ec);
    ioc.stop();
    if (thr.joinable()) thr.join();
  }
};

WebSocketClient::WebSocketClient(const std::string& h,
                                 const std::string& p,
                                 const std::string& t)
  : impl_(std::make_unique<Impl>(h,p,t)) {}
WebSocketClient::~WebSocketClient() = default;

void WebSocketClient::connect()               { impl_->connect(); }
void WebSocketClient::send(const std::string& m)    { impl_->send(m); }
void WebSocketClient::onMessage(RawMessageCallback cb) { impl_->cb = std::move(cb); }
void WebSocketClient::stop()                  { impl_->stop(); }
} // namespace PeerJS
