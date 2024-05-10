#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <fstream>
#include <iostream>
#include <string>

namespace config {

struct handler_config {
  std::string app_key;
  std::string session_token;
  std::string market_id;
};

struct config {
public:
  config(const std::string &filename) {
    config_.add_options()("handler.app_key",
                          boost::program_options::value(&handler_cfg_.app_key));
    config_.add_options()("handler.session_token", boost::program_options::value(
                                               &handler_cfg_.session_token));
    config_.add_options()("handler.market_id",
                          boost::program_options::value(&handler_cfg_.market_id));
    parse(filename);
  }

  void parse(const std::string &filename) {
    auto ifs = std::ifstream{filename};
    if (!ifs.is_open()) {
      throw std::runtime_error("Can't open config file");
    }

    boost::program_options::variables_map vm;
    boost::program_options::store(
        boost::program_options::parse_config_file(ifs, config_), vm);
    boost::program_options::notify(vm);
  }

  constexpr auto& handler_cfg() const noexcept { return handler_cfg_; }

private:
  boost::program_options::options_description config_{"Configuration"};
  handler_config handler_cfg_;
};

} // namespace config

#endif
