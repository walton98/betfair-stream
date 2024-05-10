#ifndef ASIO_UTILS_HPP
#define ASIO_UTILS_HPP

#include <exception>
#include <iostream>

namespace asio_utils {

inline void print_exception_token(std::exception_ptr eptr) {
  if (eptr) {
    try {
      std::rethrow_exception(eptr);
    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }
  }
}

} // namespace asio_utils

#endif
