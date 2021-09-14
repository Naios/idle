
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

#ifndef IDLE_INTERFACE_ARGUMENTS_HPP_INCLUDED
#define IDLE_INTERFACE_ARGUMENTS_HPP_INCLUDED

#include <string>
#include <vector>
#include <idle/core/api.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/interface/command.hpp>

namespace idle {
class IDLE_API(idle) Arguments {
  explicit Arguments(std::vector<std::string> args)
    : args_(std::move(args)) {}

  using storage_t = std::vector<std::string>;

public:
  using size_type = storage_t::size_type;

  Arguments(Arguments&&) = default;
  Arguments(Arguments const&) = default;
  Arguments& operator=(Arguments&&) = default;
  Arguments& operator=(Arguments const&) = default;

  bool empty() const noexcept {
    return args_.empty();
  }
  size_t size() const noexcept {
    return args_.size();
  }

  std::string const& get_string(std::size_t index) const;
  std::size_t get_uint(std::size_t index) const;

  /// Converts a string into arguments
  static Arguments from(StringView str);

private:
  storage_t args_;
};
} // namespace idle

#endif // IDLE_INTERFACE_ARGUMENTS_HPP_INCLUDED
