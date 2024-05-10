#include <format>
#include <iostream>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/deferred.hpp>
#include <boost/system/error_code.hpp>

#include "handler.hpp"

namespace handler {

boost::asio::awaitable<void> handler::run() {
  co_await authentication();
  co_await(subscribe_to_market(cfg_.market_id));

  while (true) {
    auto msg = co_await channel_.async_receive(boost::asio::deferred);
    co_await handle_message(msg);
  }
}

boost::asio::awaitable<void> handler::handle_message(const std::string &msg) {
  std::cout << "Received: " << msg << '\n';
  co_return;
}

boost::asio::awaitable<void> handler::subscribe_to_market(std::string_view market_id) {
  auto msg = std::format(
      R"({{"op": "marketSubscription", "id": 1, "marketFilter": {{"marketIds": ["{}"]}}, )"
      R"("marketDataFilter": {{"ladderLevels": 10, "fields": ["EX_MARKET_DEF", "EX_BEST_OFFERS"]}}}})",
      market_id);
  co_await (send(msg));
}

boost::asio::awaitable<void> handler::authentication() {
  auto msg = std::format(
      R"({{"op": "authentication", "appKey": "{}", "session": "{}"}})",
      cfg_.app_key, cfg_.session_token);
  co_await send(msg);
}


boost::asio::awaitable<void> handler::send(std::string &msg) {
  msg.append("\r\n");
  co_await stream_channel_.async_send(boost::system::error_code{}, msg,
                                      boost::asio::deferred);
}

} // namespace handler
