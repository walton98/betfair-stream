#include <iostream>

#include "asio_utils.hpp"
#include "async_stream.hpp"

namespace {

std::string parse_buffer(auto &buffer) {
  std::istream stream{&buffer};
  return std::string(std::istreambuf_iterator<char>(stream), {});
}

} // namespace

namespace stream {

stream::stream(const std::string &hostname, boost::asio::io_context &ioc,
               types::stream_channel &channel,
               types::handler_channel &handler_channel)
    : ssl_context_(boost::asio::ssl::context::tls), socket_(ioc, ssl_context_),
      resolver_(ioc), hostname_(hostname), channel_{channel},
      handler_channel_{handler_channel} {}

boost::asio::awaitable<void> stream::start() {
  co_await connect();
  boost::asio::co_spawn(socket_.get_executor(), rx(),
                        asio_utils::print_exception_token);
  boost::asio::co_spawn(socket_.get_executor(), tx(),
                        asio_utils::print_exception_token);
}

boost::asio::awaitable<void> stream::connect() {
  auto endpoints =
      co_await resolver_.async_resolve(hostname_, "443", boost::asio::deferred);
  auto &socket = socket_.next_layer();
  socket.set_option(boost::asio::ip::tcp::no_delay{true});
  co_await boost::asio::async_connect(socket, endpoints, boost::asio::deferred);
  co_await socket_.async_handshake(boost::asio::ssl::stream_base::client,
                                   boost::asio::deferred);
}

boost::asio::awaitable<void> stream::rx() {
  while (true) {
    boost::asio::streambuf receive_buffer;
    co_await boost::asio::async_read_until(socket_, receive_buffer, "\n",
                                           boost::asio::deferred);

    co_await handler_channel_.async_send(boost::system::error_code{},
                                         parse_buffer(receive_buffer),
                                         boost::asio::deferred);
  }
}

boost::asio::awaitable<void> stream::tx() {
  while (true) {
    auto msg = co_await channel_.async_receive(boost::asio::deferred);
    co_await send_msg(msg);
  }
}

boost::asio::awaitable<void> stream::send_msg(std::string_view msg) {
  std::cout << "Sending: '" << msg << "'\n";
  co_await boost::asio::async_write(socket_, boost::asio::buffer(msg),
                                    boost::asio::deferred);
}

} // namespace stream
