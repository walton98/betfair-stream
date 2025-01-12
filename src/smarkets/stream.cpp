#include "stream.hpp"
#include <boost/asio/use_awaitable.hpp>
#include <iostream>

namespace smarkets {

stream_session::stream_session(const std::string &host,
                               boost::asio::io_context &ioc,
                               boost::asio::ssl::context &ctx)
    : ws_{ioc, ctx}, resolver_{ioc}, host_{host} {}

boost::asio::awaitable<void> stream_session::connect() {
  auto const results = co_await resolver_.async_resolve(
      host_, "443", boost::asio::use_awaitable);
  // Set a timeout on the operation
  boost::beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

  // Make the connection on the IP address we get from a lookup
  auto ep = co_await boost::beast::get_lowest_layer(ws_).async_connect(
      results, boost::asio::use_awaitable);

  if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(),
                                host_.c_str())) {
    throw std::runtime_error{"poopies"};
  }

  // Update the host string. This will provide the value of the
  // Host HTTP header during the WebSocket handshake.
  // See https://tools.ietf.org/html/rfc7230#section-5.4
  // host += ':' + std::to_string(ep.port());

  // Set a timeout on the operation
  boost::beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

  // Set a decorator to change the User-Agent of the handshake
  ws_.set_option(boost::beast::websocket::stream_base::decorator(
      [](boost::beast::websocket::request_type &req) {
        req.set(boost::beast::http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                    " websocket-client-coro");
      }));

  // Perform the SSL handshake
  co_await ws_.next_layer().async_handshake(
      boost::asio::ssl::stream_base::client, boost::asio::use_awaitable);

  // Turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  boost::beast::get_lowest_layer(ws_).expires_never();

  // Set suggested timeout settings for the websocket
  ws_.set_option(boost::beast::websocket::stream_base::timeout::suggested(
      boost::beast::role_type::client));

  // Perform the websocket handshake
  std::string host = host_;
  host += ":443";
  co_await ws_.async_handshake(host_, "/", boost::asio::use_awaitable);

  // Send the message
  // co_await ws_.async_write(boost::asio::buffer(std::string("")),
  // boost::asio::use_awaitable);

  // This buffer will hold the incoming message
  boost::beast::flat_buffer buffer;

  // Read a message into our buffer
  co_await ws_.async_read(buffer, boost::asio::use_awaitable);

  // The make_printable() function helps print a ConstBufferSequence
  std::cout << boost::beast::make_printable(buffer.data()) << std::endl;
}

} // namespace smarkets
