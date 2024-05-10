#include <boost/asio/deferred.hpp>

#include <boost/asio.hpp>

#include "asio_utils.hpp"
#include "async_stream.hpp"
#include "handler.hpp"

int main() {
  boost::asio::io_context ioc;
  auto work = boost::asio::make_work_guard(ioc.get_executor());
  auto asio_thread = std::thread([&]() { ioc.run(); });

  // Create channels
  boost::asio::experimental::channel<void(boost::system::error_code,
                                          std::string)>
      handler_channel{ioc, 1024};
  boost::asio::experimental::channel<void(boost::system::error_code,
                                          std::string)>
      stream_channel{ioc, 1024};

  // Create actors
  stream::stream async_client{"stream-api-integration.betfair.com", ioc,
                              stream_channel, handler_channel};
  handler::handler handler{handler_channel, stream_channel};

  // Start actors
  boost::asio::co_spawn(ioc, async_client.start(),
                        asio_utils::print_exception_token);
  boost::asio::co_spawn(ioc, handler.run(), asio_utils::print_exception_token);

  // Wait for everything to finish
  work.reset();
  asio_thread.join();
}
