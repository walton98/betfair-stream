#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>

#include <boost/asio/experimental/channel.hpp>
#include <boost/system/error_code.hpp>

namespace types {

using handler_channel = boost::asio::experimental::channel<void(
    boost::system::error_code, std::string)>;

using stream_channel = boost::asio::experimental::channel<void(
    boost::system::error_code, std::string)>;

using market_id = std::string;
using runner_id = uint64_t;
using price = double;
using volume = double;

enum class side {
  buy,
  sell,
};

} // namespace types

#endif
