#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "types.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <boost/system/error_code.hpp>

namespace handler {

class handler {
public:
  handler(types::handler_channel &channel,
          types::stream_channel &stream_channel)
      : channel_{channel}, stream_channel_{stream_channel} {}

  boost::asio::awaitable<void> run();

private:
  types::handler_channel &channel_;
  types::stream_channel &stream_channel_;
};

} // namespace handler

#endif
