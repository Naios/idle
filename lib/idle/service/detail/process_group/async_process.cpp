
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

#include <exception>
#include <idle/service/detail/process_group/async_process.hpp>

namespace idle {
namespace detail {
class process_bad_exit_code_exception_impl final
  : public process_bad_exit_code_exception {

  int exit_code_;
  std::error_code error_code_;
  std::string message_;

public:
  explicit process_bad_exit_code_exception_impl(int exit_code,
                                                std::error_code error_code)
    : exit_code_(exit_code)
    , error_code_(std::move(error_code))
    , message_(error_code_.message()) {}

  char const* what() const noexcept override {
    return message_.c_str();
  }

  int exit_code() const override {
    return exit_code_;
  }
};

class process_error_code_exception_impl final
  : public process_execution_exception {

  std::error_code error_code_;
  std::string message_;

public:
  explicit process_error_code_exception_impl(std::error_code error_code)
    : error_code_(std::move(error_code))
    , message_(error_code_.message()) {}

  char const* what() const noexcept override {
    return message_.c_str();
  }
};

std::exception_ptr make_bad_exit_code_exception_ptr(std::error_code ec,
                                                    int exit_code) {
  return std::make_exception_ptr(
      process_bad_exit_code_exception_impl{exit_code, std::move(ec)});
}

std::exception_ptr make_error_code_exception(std::error_code ec) {
  return std::make_exception_ptr(
      process_error_code_exception_impl{std::move(ec)});
}
} // namespace detail
} // namespace idle
