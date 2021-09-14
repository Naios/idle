
/*
 *   _____    _ _        .      .    .
 *  |_   _|  | | |  .       .           .
 *    | |  __| | | ___         .    .        .
 *    | | / _` | |/ _ \                .
 *   _| || (_| | |  __/ github.com/Naios/idle
 *  |_____\__,_|_|\___| AGPL v3 (Early Access)
 *
 * Copyright(c) 2018 - 2021 Denis Blank <denis.blank at outlook dot com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IDLE_SERVICE_DETAIL_PROMISES_MAPPING_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_PROMISES_MAPPING_HPP_INCLUDED

#include <exception>
#include <system_error>
#include <boost/system/error_code.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/dep/variant.hpp>

namespace idle {
class error_code_exception : public std::exception {
  variant<std::error_code, boost::system::error_code> code_;

public:
  explicit error_code_exception(std::error_code code) noexcept
    : code_(std::move(code)) {}
  explicit error_code_exception(boost::system::error_code code) noexcept
    : code_(std::move(code)) {}

  char const* what() const noexcept override {
    return visit(
        [](auto&& ec) {
          return ec.message().c_str();
        },
        code_);
  }
};

template <typename Promise, typename ErrorCode, typename... Args>
void direct_resolve_from_boost(Promise&& promise, ErrorCode&& ec,
                               Args&&... args) {
  if (ec) {
    auto ptr = std::make_exception_ptr(
        error_code_exception(std::forward<ErrorCode>(ec)));
    std::forward<Promise>(promise).set_exception(std::move(ptr));
  } else {
    std::forward<Promise>(promise).set_value(
        std::forward<decltype(args)>(args)...);
  }
}

inline auto default_boost_asio_resolver() {
  return [](auto&& promise, boost::system::error_code ec, auto&&... args) {
    if (!ec || (ec != boost::asio::error::operation_aborted)) {
      direct_resolve_from_boost(std::forward<decltype(promise)>(promise), ec,
                                std::forward<decltype(args)>(args)...);
    }
  };
}

inline auto ignore_error_code_boost_resolver(int code) {
  return [code](auto&& promise, boost::system::error_code ec, auto&&... args) {
    if (ec.value() == code) {
      ec.assign(0, ec.category());
    }
    default_boost_asio_resolver()(std::forward<decltype(promise)>(promise),
                                  std::move(ec),
                                  std::forward<decltype(args)>(args)...);
  };
}
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_PROMISES_MAPPING_HPP_INCLUDED
