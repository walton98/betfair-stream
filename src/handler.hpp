#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <array>
#include <format>
#include <unordered_map>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <boost/system/error_code.hpp>
#include <nlohmann/json.hpp>

#include "config.hpp"
#include "types.hpp"

namespace handler {

constexpr size_t DEPTH = 10;

struct book_side {
  std::array<std::pair<types::price, types::volume>, DEPTH> data;
};

class runner {
public:
  runner(types::runner_id id) : id_{id} {}

  constexpr types::runner_id get_id() const noexcept { return id_; }

  constexpr auto &buyers() const noexcept { return buy_; }

  constexpr auto &sellers() const noexcept { return sell_; }

  void update_volume(types::side, size_t idx, types::price, types::volume);

private:
  types::runner_id id_;
  // People who are buying
  book_side buy_;
  // People who are selling
  book_side sell_;
};

class market {
public:
  void insert_runner(types::runner_id runner_id) {
    books_.emplace(runner_id, runner{runner_id});
  }

  auto &runners() const noexcept { return books_; }

  auto &get_runner(types::runner_id runner_id) { return books_.at(runner_id); }

private:
  std::unordered_map<types::runner_id, runner> books_{};
};

class market_manager {
public:
  void handle_market_definition(const types::market_id &market_id,
                                const nlohmann::json &market_def);

  void handle_runner_change(const types::market_id &, const nlohmann::json &);

  auto &markets() const noexcept { return markets_; }

private:
  std::unordered_map<types::market_id, market> markets_;
};

class handler {
public:
  handler(types::handler_channel &, types::stream_channel &,
          const config::handler_config &);

  boost::asio::awaitable<void> run();

private:
  boost::asio::awaitable<void> handle_message(const std::string &);
  boost::asio::awaitable<void> authentication();
  boost::asio::awaitable<void> subscribe_to_market(std::string_view market_id);
  boost::asio::awaitable<void> send(std::string &msg);

  types::handler_channel &channel_;
  types::stream_channel &stream_channel_;
  config::handler_config cfg_;
  market_manager markets_;
};

} // namespace handler

template <> struct std::formatter<handler::book_side> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const handler::book_side &bs, std::format_context &ctx) const {
    std::format_to(ctx.out(), "[ ");
    for (const auto [price, vol] : bs.data) {
      if (price != 0 && vol != 0) {
        std::format_to(ctx.out(), "[{}, {}] ", price, vol);
      }
    }
    return std::format_to(ctx.out(), "]");
  }
};

template <> struct std::formatter<handler::runner> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const handler::runner &runner, std::format_context &ctx) const {
    return std::format_to(ctx.out(), "Buyers: {}\nSellers: {}\n",
                          runner.buyers(), runner.sellers());
  }
};

template <> struct std::formatter<handler::market> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const handler::market &market, std::format_context &ctx) const {
    for (const auto &[id, runner] : market.runners()) {
      std::format_to(ctx.out(), "Runner {}:\n{}", id, runner);
    }
    return ctx.out();
  }
};

template <> struct std::formatter<handler::market_manager> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const handler::market_manager &manager,
              std::format_context &ctx) const {
    for (const auto &[id, market] : manager.markets()) {
      std::format_to(ctx.out(), "Market {}:\n{}\n", id, market);
    }
    return ctx.out();
  }
};

#endif
