
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

#ifndef IDLE_CORE_UTIL_TYPE_NAME_HPP_INCLUDED
#define IDLE_CORE_UTIL_TYPE_NAME_HPP_INCLUDED

#include <cstddef>
#include <iosfwd>
#include <idle/core/detail/type_name_impl.hpp>
#include <idle/core/util/array.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
template <std::size_t Size>
class StaticTypeName {
  friend class TypeName;

  static_assert(Size > 0U, "Something went wrong here!");

public:
  constexpr StaticTypeName(char const* begin, char const* end) noexcept
    : type_(begin, end) {}

  template <std::size_t Other>
  constexpr bool operator==(StaticTypeName<Other> const&) const noexcept {
    return false;
  }
  constexpr bool operator==(StaticTypeName<Size> const& other) const noexcept {
    return StringView(*this) == StringView(other);
  }

  constexpr char const* begin() const noexcept {
    return type_.data();
  }
  constexpr char const* end() const noexcept {
    return type_.data() + Size;
  }
  constexpr char const* data() const noexcept {
    return type_.data();
  }
  constexpr bool empty() const noexcept {
    return false;
  }
  constexpr std::size_t size() const noexcept {
    return Size;
  }

  friend std::ostream& operator<<(std::ostream& os, StaticTypeName const& t) {
    return os << StringView(t.type_);
  }

private:
  Array<char, Size> type_;
};

class TypeName {
public:
  template <std::size_t Size>
  explicit constexpr TypeName(StaticTypeName<Size> const& name)
    : type_(name.data(), name.data() + Size) {}

  bool operator==(TypeName const& other) const noexcept {
    return type_ == other.type_;
  }

  char const* begin() const noexcept {
    return type_.data();
  }
  char const* end() const noexcept {
    return type_.data() + type_.size();
  }
  char const* data() const noexcept {
    return type_.data();
  }
  bool empty() const noexcept {
    return type_.empty();
  }
  std::size_t size() const noexcept {
    return type_.size();
  }

  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os,
                                                 TypeName const& t);

private:
  std::string type_;
};

/// Returns the name of the given type
template <typename T>
constexpr auto type_name() noexcept {
  constexpr StringView name = detail::type_name_impl<T>();
  constexpr StaticTypeName<name.size()> type(name.data(),
                                             name.data() + name.size());
  return type;
}
} // namespace idle

#endif // IDLE_CORE_UTIL_TYPE_NAME_HPP_INCLUDED
