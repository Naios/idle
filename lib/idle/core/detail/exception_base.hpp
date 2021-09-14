
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

#ifndef IDLE_CORE_DETAIL_EXCEPTION_BASE_HPP_INCLUDED
#define IDLE_CORE_DETAIL_EXCEPTION_BASE_HPP_INCLUDED

#include <system_error>
#include <boost/exception/exception.hpp>
// #include <idle/core/exception.hpp>

namespace idle {
template <typename Base, typename Exception>
class exception_wrapper : public Base {
  Exception exception_;

public:
  explicit exception_wrapper(Exception exception)
    : exception_(std::move(exception_)) {}

  char const* what() const noexcept {
    return exception_.what();
  }
};

template <typename Base, typename Exception>
class encoded_exception_wrapper : public Base {
  Exception exception_;

public:
  explicit encoded_exception_wrapper(Exception exception)
    : exception_(std::move(exception)) {}

  char const* what() const noexcept override {
    return exception_.what();
  }

  std::error_code const& error_code() const override {
    return exception_.code();
  }
};

template <typename Base>
class error_code_wrapper : public Base {
  std::error_code code_;

public:
  explicit error_code_wrapper(std::error_code code)
    : code_(std::move(code)) {}

  char const* what() const noexcept override {
    return code_.message().c_str();
  }

  std::error_code const& error_code() const override {
    return code_;
  }
};
} // namespace idle

#endif // IDLE_CORE_DETAIL_EXCEPTION_BASE_HPP_INCLUDED
