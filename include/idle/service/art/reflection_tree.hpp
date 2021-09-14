
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

#ifndef IDLE_SERVICE_ART_REFLECTION_TREE_HPP_INCLUDED
#define IDLE_SERVICE_ART_REFLECTION_TREE_HPP_INCLUDED

#include <type_traits>
#include <idle/core/util/meta.hpp>
#include <idle/service/art/reflection.hpp>
#include <idle/service/art/types.hpp>
#include <idle/service/detail/art/reflection_tree_impl.hpp>

#ifndef IDLE_HAS_REFLECT_LIMIT
#  include <idle/service/detail/art/reflect_64.inl>
#else
#  if IDLE_HAS_REFLECT_LIMIT <= 64
#    include <idle/service/detail/art/reflect_64.inl>
#  elif IDLE_HAS_REFLECT_LIMIT <= 128
#    include <idle/service/detail/art/reflect_128.inl>
#  elif IDLE_HAS_REFLECT_LIMIT <= 256
#    include <idle/service/detail/art/reflect_256.inl>
#  elif IDLE_HAS_REFLECT_LIMIT <= 512
#    include <idle/service/detail/art/reflect_512.inl>
#  else // IDLE_HAS_REFLECT_LIMIT > 512
#    error idle::reflect does support 512 args max!
#  endif
#endif

namespace idle {
template <typename T, typename... Fields>
constexpr auto reflection(Fields&&... fields) noexcept {
  return art::detail::ReflectionImpl<T, sizeof...(Fields)>(
      {std::forward<Fields>(fields)...});
}

template <typename T, typename Class>
art::FieldType field(char const* name, T Class::*accessor,
                     char const* comment = {}) noexcept {
  // TODO Improve this once offset calculation can be constexpr
  std::size_t const offset = reinterpret_cast<std::size_t>(
      std::addressof(static_cast<Class*>(nullptr)->*accessor));

  constexpr art::TypeOf<T> type;
  return art::FieldType(type(), art::detail::subtypeSelect(type, identity<T>{}),
                        name, comment, offset);
}
} // namespace idle

#define IDLE_DETAIL_FIELD(TYPE, x)                                             \
  IDLE_DETAIL_IIF(IDLE_DETAIL_IS_PAREN(x))                                     \
  (::idle::field(IDLE_DETAIL_STRINGIFY(IDLE_DETAIL_ARGS_FIRST x),              \
                 &TYPE::IDLE_DETAIL_ARGS_FIRST x, IDLE_DETAIL_ARGS_REST x),    \
   ::idle::field(#x, &TYPE::x))

/// Defines the reflection function of type TYPE:
/// ```
/// idle::Reflection const& reflect(TYPE const*) noexcept
/// ```
///
/// Can be used as following:
/// ```
/// struct MyStruct {
///   int a;
///   std::string somestr;
///   std::vector<int> ints;
/// };
///
/// IDLE_REFLECT(MyStruct, a, somestr, (ints, "comment on ints"))
///
/// void do_sth() {
///   Reflection& ref = reflect(static_cast<MyStruct>(nullptr));
///   // ...
/// }
/// ```
///
/// The macro accepts the reflected types as well as the names
/// of its reflected fields.
///
/// A type reflected by IDLE_REFLECT can also specify its primary base class
/// through a Super typedef. The primary base classes reflection is then
/// returned by \see Reflection::super and taken into account during
/// a \see reflection_visit.
///
/// \note The IDLE_REFLECT macro can also be prefixed with a specifier such as
///       inline, attributes or export specifiers such as IDLE_API(...).
#define IDLE_REFLECT(TYPE, ...)                                                \
  ::idle::Reflection const& reflect(TYPE const*) noexcept {                    \
    static auto const refl = ::idle::reflection<TYPE>(                         \
        IDLE_DETAIL_REFLECT(TYPE, __VA_ARGS__));                               \
    return refl;                                                               \
  }

#endif // IDLE_SERVICE_ART_REFLECTION_TREE_HPP_INCLUDED
