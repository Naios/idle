
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

#ifndef IDLE_CORE_UTIL_ITERATOR_FACADE_HPP_INCLUDED
#define IDLE_CORE_UTIL_ITERATOR_FACADE_HPP_INCLUDED

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <idle/core/util/assert.hpp>

namespace idle {
namespace detail {
template <typename Reference, typename Pointer, typename = void>
struct arrow_op {
  static constexpr Pointer apply(Reference ref) {
    return std::addressof(ref);
  }
};

template <typename Reference, typename Pointer>
struct arrow_op<Reference, Pointer,
                std::enable_if_t<std::is_pointer<Reference>::value>> {
  static constexpr Pointer apply(Reference ref) {
    return ref;
  }
};

template <typename Derived, typename Trait>
struct iterator_facade_base_bidirectional {
  constexpr Derived& operator--() {
    static_cast<Derived*>(this)->decrement();
    return *static_cast<Derived*>(this);
  }

  constexpr Derived operator--(int) {
    Derived tmp(*static_cast<Derived*>(this));
    static_cast<Derived*>(this)->decrement();
    return tmp;
  }
};

template <typename Derived, typename Trait>
struct iterator_facade_base_random_access {
  constexpr Derived operator+(typename Trait::difference_type n) const {
    Derived tmp(*static_cast<Derived const*>(this));
    tmp.advance(n);
    return tmp;
  }

  constexpr Derived operator-(typename Trait::difference_type n) const {
    Derived tmp(*static_cast<Derived const*>(this));
    tmp.advance(-n);
    return tmp;
  }

  constexpr Derived& operator+=(typename Trait::difference_type n) {
    static_cast<Derived*>(this)->advance(n);
    return *static_cast<Derived*>(this);
  }

  constexpr Derived& operator-=(typename Trait::difference_type n) {
    static_cast<Derived*>(this)->advance(-n);
    return *static_cast<Derived*>(this);
  }

  constexpr typename Trait::reference
  operator[](typename Trait::difference_type n) const {
    Derived tmp(*static_cast<Derived const*>(this));
    tmp.advance(n);
    return tmp.dereference();
  }

  constexpr typename Trait::difference_type
  operator-(Derived const& right) const {
    return -(static_cast<Derived const*>(this)->distance_to(right));
  }

  constexpr bool operator<(Derived const& right) const {
    return static_cast<Derived const*>(this)->distance_to(right) > 0;
  }

  constexpr bool operator>(Derived const& right) const {
    return static_cast<Derived const*>(this)->distance_to(right) < 0;
  }

  constexpr bool operator>=(Derived const& right) const {
    return !operator<(right);
  }

  constexpr bool operator<=(Derived const& right) const {
    return !operator>(right);
  }
};

template <typename Derived, typename Trait, typename Tag>
struct iterator_facade_base;
template <typename Derived, typename Trait>
struct iterator_facade_base<Derived, Trait, std::forward_iterator_tag> {};
template <typename Derived, typename Trait>
struct iterator_facade_base<Derived, Trait, std::bidirectional_iterator_tag>
  : iterator_facade_base_bidirectional<Derived, Trait> {};
template <typename Derived, typename Trait>
struct iterator_facade_base<Derived, Trait, std::random_access_iterator_tag>
  : iterator_facade_base_bidirectional<Derived, Trait>,
    iterator_facade_base_random_access<Derived, Trait> {};

template <typename Category, typename Value, typename Difference,
          typename Pointer, typename Reference>
struct iterator_trait {
  using size_type = std::size_t;
  using iterator_category = Category;
  using difference_type = Difference;
  using value_type = Value;
  using pointer = Pointer;
  using reference = Reference;
};
} // namespace detail

template <typename Derived, typename Category, typename Value,
          typename Difference = std::ptrdiff_t,
          typename Pointer = std::remove_reference_t<Value>*,
          typename Reference = std::remove_reference_t<Value>&,
          typename Trait = detail::iterator_trait<Category, Value, Difference,
                                                  Pointer, Reference>>
struct iterator_facade
  : public Trait,
    public detail::iterator_facade_base<Derived, Trait, Category> {

  constexpr bool operator==(iterator_facade const& other) const noexcept {
    return derived().equal(other.derived());
  }
  constexpr bool operator!=(iterator_facade const& other) const noexcept {
    return !derived().equal(other.derived());
  }

  constexpr Derived& operator++() noexcept {
    derived().increment();
    return derived();
  }

  constexpr Derived operator++(int) noexcept {
    Derived tmp(derived());
    derived().increment();
    return tmp;
  }

  constexpr typename Trait::reference operator*() const noexcept {
    return derived().dereference();
  }

  constexpr typename Trait::pointer operator->() const noexcept {
    return detail::arrow_op<typename Trait::reference,
                            typename Trait::pointer>::apply(derived()
                                                                .dereference());
  }

private:
  constexpr Derived& derived() noexcept {
    return *static_cast<Derived*>(this);
  }
  constexpr Derived const& derived() const noexcept {
    return *static_cast<Derived const*>(this);
  }
};
} // namespace idle

#endif // IDLE_CORE_UTIL_ITERATOR_FACADE_HPP_INCLUDED
