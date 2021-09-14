
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

#ifndef IDLE_CORE_UTIL_RANGE_HPP_INCLUDED
#define IDLE_CORE_UTIL_RANGE_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <idle/core/detail/algorithm.hpp>

namespace idle {
template <typename Iterator, typename SizeType = void>
class Range;

namespace detail {
template <typename Iterator>
struct range_base : public std::pair<Iterator, Iterator> {
  using iterator = Iterator;

  using std::pair<Iterator, Iterator>::pair;

  constexpr Iterator begin() const noexcept {
    return this->first;
  }
  constexpr Iterator end() const noexcept {
    return this->second;
  }
  constexpr bool empty() const noexcept {
    return this->first == this->second;
  }
  constexpr explicit operator bool() const noexcept {
    return !empty();
  }
  constexpr decltype(auto) front() const noexcept {
    IDLE_ASSERT(!empty());
    return *this->first;
  }
};
} // namespace detail

template <typename Iterator, typename SizeType>
class Range : public detail::range_base<Iterator> {
public:
  using size_type = SizeType;

  /* implicit */ constexpr Range() = default;
  /* implicit */ constexpr Range(Iterator begin, Iterator end, size_type size)
    : detail::range_base<Iterator>(std::move(begin), std::move(end))
    , size_(size) {}

  constexpr size_type size() const noexcept {
    return size_;
  }
  constexpr Range next() const noexcept {
    IDLE_ASSERT(!this->empty());
    Iterator n = this->begin();
    return {++n, this->end(), size_ - 1};
  }

private:
  std::size_t size_{0U};
};
template <typename Iterator>
class Range<Iterator, void> : public detail::range_base<Iterator> {
public:
  using size_type = std::size_t;

  /* implicit */ constexpr Range() = default;
  /* implicit */ constexpr Range(Iterator begin, Iterator end)
    : detail::range_base<Iterator>(std::move(begin), std::move(end)) {}

  constexpr size_type size() const noexcept {
    return detail::distance(this->first, this->second);
  }
  constexpr Range next() const noexcept {
    IDLE_ASSERT(!this->empty());
    Iterator n = this->begin();
    return {++n, this->end()};
  }
};

namespace detail {
template <std::size_t I>
struct range_view_access;
template <>
struct range_view_access<0U> {
  template <typename T>
  static auto const& retrieve(T&& view) noexcept {
    return std::forward<T>(view).first;
  }
};
template <>
struct range_view_access<1U> {
  template <typename T>
  static auto const& retrieve(T&& view) noexcept {
    return std::forward<T>(view).second;
  }
};
} // namespace detail

/// Returns the iterator at position I of the given result
template <std::size_t I, typename Iterator, typename SizeType>
Iterator get(Range<Iterator, SizeType>& view) {
  return detail::range_view_access<I>::retrieve(view);
}
/// \copydoc get
template <std::size_t I, typename Iterator, typename SizeType>
Iterator get(Range<Iterator, SizeType> const& view) {
  return detail::range_view_access<I>::retrieve(view);
}
/// \copydoc get
template <std::size_t I, typename Iterator, typename SizeType>
Iterator get(Range<Iterator, SizeType>&& view) {
  return detail::range_view_access<I>::retrieve(std::move(view));
}

template <
    typename Iterable,
    typename Range = Range<typename std::decay_t<Iterable>::iterator, void>>
constexpr Range make_range(Iterable&& iterable) noexcept {
  return {iterable.begin(), iterable.end()};
}
template <typename Iterator,
          typename Range = Range<std::decay_t<Iterator>, void>>
constexpr Range make_range(Iterator begin, Iterator end) noexcept {
  return {begin, end};
}
template <
    typename Iterator, typename SizeType,
    typename Range = Range<std::decay_t<Iterator>, std::decay_t<SizeType>>>
constexpr Range make_range(Iterator begin, Iterator end,
                           SizeType size) noexcept {
  return {begin, end, size};
}
} // namespace idle

namespace std {
// The GCC standard library defines tuple_size as class and struct which
// triggers a warning here.
#if defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmismatched-tags"
#endif
template <typename Iterator, typename SizeType>
struct tuple_size<idle::Range<Iterator, SizeType>>
  : std::integral_constant<size_t, 2> {};

template <std::size_t I, typename Iterator, typename SizeType>
struct tuple_element<I, idle::Range<Iterator, SizeType>> {
  using type = Iterator;
};
#if defined(__clang__)
#  pragma GCC diagnostic pop
#endif
} // namespace std

#endif // IDLE_CORE_UTIL_RANGE_HPP_INCLUDED
