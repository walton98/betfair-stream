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

} // namespace types

#endif
