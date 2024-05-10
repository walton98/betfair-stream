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
  co_await boost::asio::async_connect(socket_.next_layer(), endpoints,
                                      boost::asio::deferred);
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
