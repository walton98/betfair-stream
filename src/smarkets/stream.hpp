#ifndef SMARKETS_STREAM_HPP
#define SMARKETS_STREAM_HPP

#include <boost/asio/awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <string>

namespace smarkets {

class stream_session {
private:
  boost::asio::ip::tcp::resolver resolver_;
  boost::beast::websocket::stream<
      boost::beast::ssl_stream<boost::beast::tcp_stream>>
      ws_;
  std::string host_;

public:
  explicit stream_session(const std::string &host,
                          boost::asio::io_context &ioc, boost::asio::ssl::context &ctx);
  void start();
  boost::asio::awaitable<void> connect();
};

} // namespace smarkets

#endif // SMARKETS_STREAM_HPP
