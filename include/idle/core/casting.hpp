
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

#ifndef IDLE_CORE_CASTING_HPP_INCLUDED
#define IDLE_CORE_CASTING_HPP_INCLUDED

#include <cstddef>
#include <memory>
#include <type_traits>
#include <idle/core/ref.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/nullable.hpp>

namespace idle {
template <typename To, typename From>
bool isa(From* source) noexcept {
  IDLE_ASSERT(source);
  return To::classof(*source);
}
template <typename To, typename From>
bool isa(From& source) noexcept {
  return To::classof(source);
}
template <typename To, typename From,
          std::enable_if_t<std::is_base_of<From, To>::value>* = nullptr>
bool isa(Ref<From> const& source) noexcept {
  if (source) {
    return isa<From>(*source);
  } else {
    return false;
  }
}

template <typename To, typename From,
          typename Dest =
              std::conditional_t<std::is_const<From>::value, To const, To>>
Dest* cast(From* source) noexcept {
  IDLE_ASSERT(isa<To>(source) && "Bad cast");
  return static_cast<Dest*>(source);
}
template <typename To, typename From,
          typename Dest =
              std::conditional_t<std::is_const<From>::value, To const, To>>
Dest& cast(From& source) noexcept {
  IDLE_ASSERT(isa<To>(source) && "Bad cast");
  return static_cast<Dest&>(source);
}
template <typename To, typename From,
          std::enable_if_t<std::is_base_of<From, To>::value>* = nullptr>
Ref<To> cast(Ref<From> source) noexcept {
  if (source) {
    IDLE_ASSERT(isa<To>(*source) && "Bad service cast");
    return std::move(source).template cast<To>();
  } else {
    return {};
  }
}

template <typename To, typename From,
          typename Dest =
              std::conditional_t<std::is_const<From>::value, To const, To>>
Nullable<Dest> dyn_cast(From* source) noexcept {
  if (isa<To>(source)) {
    return Nullable<Dest>(*static_cast<Dest*>(source));
  } else {
    return {};
  }
}
template <typename To, typename From,
          typename Dest =
              std::conditional_t<std::is_const<From>::value, To const, To>>
Nullable<Dest> dyn_cast(From& source) noexcept {
  if (isa<To>(source)) {
    return Nullable<Dest>(*static_cast<Dest*>(std::addressof(source)));
  } else {
    return {};
  }
}
template <typename To, typename From,
          typename Dest =
              std::conditional_t<std::is_const<From>::value, To const, To>>
Ref<Dest> dyn_cast(Ref<From> service) noexcept {
  if (isa<To>(service)) {
    return std::move(service).template cast<To>();
  } else {
    return {};
  }
}
} // namespace idle

#endif // IDLE_CORE_CASTING_HPP_INCLUDED
