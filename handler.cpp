#include <iostream>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/deferred.hpp>
#include <boost/system/error_code.hpp>

#include "handler.hpp"

namespace handler {

boost::asio::awaitable<void> handler::run() {
  std::string msg = "{\"op\": \"authentication\", \"appKey\": \"...\", "
                    "\"session\": \"...\"}\r\n";
  co_await stream_channel_.async_send(boost::system::error_code{}, msg,
                                      boost::asio::deferred);

  const std::string market_id = "1.227301258";
  msg =
      R"({"op": "marketSubscription", "id": 1, "marketFilter": { "marketIds": [")" +
      market_id +
      R"(" ] }, "marketDataFilter": { "ladderLevels": 10, "fields": [ "EX_MARKET_DEF", "EX_BEST_OFFERS" ] } })" +
      "\r\n";
  co_await stream_channel_.async_send(boost::system::error_code{}, msg,
                                      boost::asio::deferred);

  while (true) {
    auto result = co_await channel_.async_receive(boost::asio::deferred);
    std::cout << "Got: " << result << '\n';
  }
}

} // namespace handler
