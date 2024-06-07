#include <format>
#include <iostream>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/deferred.hpp>
#include <boost/system/error_code.hpp>
#include <nlohmann/json.hpp>

#include "handler.hpp"

namespace handler {

handler::handler(types::handler_channel &channel,
                 types::stream_channel &stream_channel,
                 const config::handler_config &cfg)
    : channel_{channel}, stream_channel_{stream_channel}, cfg_{cfg} {}

boost::asio::awaitable<void> handler::run() {
  co_await authentication();
  co_await (subscribe_to_market(cfg_.market_id));

  while (true) {
    auto msg = co_await channel_.async_receive(boost::asio::deferred);
    co_await handle_message(msg);
  }
}

boost::asio::awaitable<void> handler::handle_message(const std::string &msg) {
  auto data = nlohmann::json::parse(msg);
  auto op = data.at("op").get<std::string>();
  if (op == "mcm") {
    auto change_type = data.value("ct", "UPDATE");
    if (change_type == "SUB_IMAGE" || change_type == "UPDATE") {
      std::cout << "Market change message: " << data << '\n';
      for (const auto &market : data.at("mc")) {
        auto market_id = market.at("id").get<types::market_id>();
        if (market.contains("marketDefinition")) {
          const auto &market_definition = market["marketDefinition"];
          markets_.handle_market_definition(market_id, market_definition);
        }
        for (const auto &runner_change : market.at("rc")) {
          markets_.handle_runner_change(market_id, runner_change);
        }
      }
      std::cout << std::format("State:\n{}\n", markets_) << '\n';
      ;
    } else if (change_type == "HEARTBEAT") {
      // Do nothing
    } else {
      std::cout << "Unsupported change type: " << change_type << '\n';
    }

  } else {
    std::cout << "Received: " << data << '\n';
  }
  co_return;
}

boost::asio::awaitable<void>
handler::subscribe_to_market(std::string_view market_id) {
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

void market_manager::handle_market_definition(
    const types::market_id &market_id, const nlohmann::json &market_def) {
  auto &market = markets_[market_id];
  for (const auto &runner : market_def.at("runners")) {
    if (runner.at("status").get<std::string>() == "ACTIVE") {
      auto runner_id = runner.at("id").get<types::runner_id>();
      market.insert_runner(runner_id);
    }
  }
}

void market_manager::handle_runner_change(const types::market_id &market_id,
                                          const nlohmann::json &runner_change) {
  auto runner_id = runner_change.at("id").get<types::runner_id>();
  auto &runner = markets_.at(market_id).get_runner(runner_id);
  auto update_side = [&](const auto &key, const auto &side) {
    if (runner_change.contains(key)) {
      for (const auto &update : runner_change[key]) {
        auto idx = update.at(0).template get<size_t>();
        auto price = update.at(1).template get<types::price>();
        auto volume = update.at(2).template get<types::volume>();
        runner.update_volume(side, idx, price, volume);
      }
    }
  };
  update_side("batb", types::side::sell);
  update_side("batl", types::side::buy);
}

void runner::update_volume(types::side side, size_t idx, types::price price,
                           types::volume volume) {
  switch (side) {
  case types::side::buy:
    buy_.data[idx] = {price, volume};
    break;
  case types::side::sell:
    sell_.data[idx] = {price, volume};
    break;
  }
}

} // namespace handler
