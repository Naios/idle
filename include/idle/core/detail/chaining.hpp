
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

#ifndef IDLE_CORE_DETAIL_CHAINING_HPP_INCLUDED
#define IDLE_CORE_DETAIL_CHAINING_HPP_INCLUDED

#include <memory>
#include <type_traits>
#include <utility>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/meta.hpp>

namespace idle {
namespace detail {
template <typename T, typename = void>
struct chain_trait {
  using type = std::add_pointer_t<T>;
  static constexpr bool value = false;

  static constexpr type do_chain(std::add_lvalue_reference_t<T> obj) noexcept {
    return std::addressof(obj);
  }
};
template <typename T>
struct chain_trait<T, void_t<decltype(std::declval<T&>().operator->())>> {
  using type = std::add_lvalue_reference_t<T>;
  static constexpr bool value = true;

  static constexpr type do_chain(type obj) noexcept {
    return obj;
  }
};

template <typename T>
struct chain_nothing {
  using type = T;

  static constexpr T do_chain(T obj) noexcept {
    return obj;
  }
};

template <typename T>
struct def_chain_traits {
  using mutable_trait = detail::chain_trait<T>;
  using const_trait = detail::chain_trait<T const>;

  static_assert(
      mutable_trait::value == const_trait::value,
      "The type T has a mismatching operator-> behaviour because of "
      "const correctness. Consider implementing an operator-> for both cases "
      "or qualify operator-> as const!");
};

template <typename T>
T* chain(T* obj) noexcept {
  IDLE_ASSERT(obj);
  return obj;
}
template <typename T>
decltype(auto) chain(T& obj) noexcept {
  return chain_trait<unrefcv_t<T>>::do_chain(obj);
}
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_CHAINING_HPP_INCLUDED
