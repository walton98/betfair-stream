#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <boost/system/error_code.hpp>

#include "config.hpp"
#include "types.hpp"

namespace handler {

class handler {
public:
  handler(types::handler_channel &channel,
          types::stream_channel &stream_channel, const config::handler_config &cfg)
      : channel_{channel}, stream_channel_{stream_channel}, cfg_{cfg} {}

  boost::asio::awaitable<void> run();

private:
  boost::asio::awaitable<void> handle_message(const std::string &);
  boost::asio::awaitable<void> authentication();
  boost::asio::awaitable<void> subscribe_to_market(std::string_view market_id);
  boost::asio::awaitable<void> send(std::string &msg);

  types::handler_channel &channel_;
  types::stream_channel &stream_channel_;
  config::handler_config cfg_;
};

} // namespace handler

#endif
