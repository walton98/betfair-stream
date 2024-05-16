#ifndef ASYNC_STREAM_HPP
#define ASYNC_STREAM_HPP

#include <string_view>

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/ssl.hpp>

#include "types.hpp"

namespace stream {

class stream {
public:
  stream(const std::string &hostname, boost::asio::io_context &,
         types::stream_channel &,
         types::handler_channel &);

  boost::asio::awaitable<void> start();
  boost::asio::awaitable<void> rx();
  boost::asio::awaitable<void> tx();

  boost::asio::awaitable<void> connect();

  boost::asio::awaitable<void> send_msg(std::string_view msg);

private:
  boost::asio::ssl::context ssl_context_;
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
  boost::asio::ip::tcp::resolver resolver_;
  std::string hostname_;
  types::stream_channel &channel_;
  types::handler_channel &handler_channel_;
};

} // namespace stream

#endif
