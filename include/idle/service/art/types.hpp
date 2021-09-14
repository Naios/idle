
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

#ifndef IDLE_SERVICE_ART_TYPES_HPP_INCLUDED
#define IDLE_SERVICE_ART_TYPES_HPP_INCLUDED

#include <chrono>
#include <initializer_list>
#include <iterator>
#include <string>
#include <type_traits>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/util/meta.hpp>
#include <idle/service/art/reflection.hpp>

namespace idle {
namespace art {
template <typename T>
struct TypeOf;

namespace detail {
/// C++17 polyfill (without initializer list)
template <typename C>
constexpr auto data(C& c) -> decltype(c.data()) {
  return c.data();
}
template <typename C>
constexpr auto data(C const& c) -> decltype(c.data()) {
  return c.data();
}
template <typename T, std::size_t N>
constexpr T* data(T (&array)[N]) noexcept {
  return array;
}

template <typename T, typename = void>
struct SelectStructuredType
  : std::integral_constant<MappedType, MappedType::Object> {};
template <typename T>
struct SelectStructuredType<
    T, void_t<decltype(::idle::art::detail::data(
           std::declval<std::add_lvalue_reference_t<T>>()))>>
  : std::integral_constant<MappedType, MappedType::Array> {};
template <typename T>
struct SelectStructuredType<
    T, void_t<decltype(std::declval<std::add_lvalue_reference_t<T>>().insert(
           *std::declval<std::add_lvalue_reference_t<T>>().begin()))>>
  : std::integral_constant<MappedType, MappedType::Set> {};
template <typename T>
struct SelectStructuredType<T, std::enable_if_t<std::is_enum<T>::value>>
  : TypeOf<std::underlying_type_t<T>> {};
} // namespace detail

template <typename T>
struct TypeOf : detail::SelectStructuredType<T> {};
#define IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(NAME, TYPE)                      \
  template <>                                                                  \
  struct TypeOf<TYPE> : std::integral_constant<MappedType, MappedType::NAME> { \
  };
#include <idle/service/art/reflection.inl>

namespace detail {
template <typename From, typename To>
using remap_to = std::conditional_t<std::is_const<From>::value, To const*, To*>;
}

template <typename T, typename Callable>
decltype(auto) type_cast(MappedType type, T* ptr, Callable&& callable) {
  switch (type) {
#define IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(NAME, TYPE)                      \
  case MappedType::NAME: {                                                     \
    return std::forward<Callable>(callable)(                                   \
        static_cast<detail::remap_to<T, TYPE>>(ptr));                          \
  }
#include <idle/service/art/reflection.inl>
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

inline void type_copy(MappedType type, void* to, void const* from) {
  type_cast(type, to, [from](auto* to_c) {
    *to_c = *static_cast<std::remove_pointer_t<decltype(to_c)> const*>(from);
  });
}

inline void type_move(MappedType type, void* to, void* from) {
  type_cast(type, to, [from](auto* to_c) {
    *to_c = std::move(
        *static_cast<std::remove_pointer_t<decltype(to_c)>*>(from));
  });
}
} // namespace art
} // namespace idle

#endif // IDLE_SERVICE_ART_TYPES_HPP_INCLUDED
