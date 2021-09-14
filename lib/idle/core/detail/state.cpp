
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

#include <ostream>
#include <idle/core/detail/state.hpp>
#include <idle/core/detail/unreachable.hpp>

namespace idle {
namespace detail {
std::ostream& operator<<(std::ostream& os, override_t value) {
  switch (value) {
#define IDLE_DETAIL_FOR_EACH_OVERRIDE(NAME)                                    \
  case override_t::NAME:                                                       \
    os << #NAME;                                                               \
    break;
#include <idle/core/detail/state.inl>
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, target_t value) {
  switch (value) {
#define IDLE_DETAIL_FOR_EACH_TARGET(NAME)                                      \
  case target_t::NAME:                                                         \
    os << #NAME;                                                               \
    break;
#include <idle/core/detail/state.inl>
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
  return os;
}
} // namespace detail
} // namespace idle
