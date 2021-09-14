
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

#ifndef IDLE_INTERFACE_PROGRAM_ARGUMENTS_HPP_INCLUDED
#define IDLE_INTERFACE_PROGRAM_ARGUMENTS_HPP_INCLUDED

#include <vector>
#include <idle/core/api.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
class IDLE_API(idle) ProgramArguments : public Interface {
  using type_t = std::vector<StringView>;

public:
  ProgramArguments(Service& owner, int argc, char** argv) noexcept;

  bool empty() const noexcept {
    return args_.empty();
  }
  std::size_t size() const noexcept {
    return args_.size();
  }

  StringView operator[](std::size_t index) const noexcept {
    IDLE_ASSERT(index < size());
    return args_[index];
  }

  type_t::const_iterator begin() const noexcept {
    return args_.begin();
  }
  type_t::const_iterator end() const noexcept {
    return args_.end();
  }

private:
  std::vector<StringView> args_;

  IDLE_INTERFACE
};
} // namespace idle

#endif // IDLE_INTERFACE_PROGRAM_ARGUMENTS_HPP_INCLUDED
