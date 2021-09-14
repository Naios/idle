
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

#ifndef IDLE_CORE_DETAIL_VIEWS_FILTER_IMPL_HPP_INCLUDED
#define IDLE_CORE_DETAIL_VIEWS_FILTER_IMPL_HPP_INCLUDED

#include <cstddef>
#include <type_traits>
#include <utility>
#include <idle/core/util/iterator_facade.hpp>
#include <idle/core/util/meta.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
namespace detail {
template <typename Itr, typename Filter>
void advance_filtering(Itr& begin, Itr end, Filter&& filter) {
  while (begin != end) {
    if (bool(filter(*begin))) {
      return;
    }
    ++begin;
  }
}

template <typename Itr, typename Filter>
class filtering_iterator
  : public Filter,
    public iterator_facade<filtering_iterator<Itr, Filter>,
                           std::forward_iterator_tag,
                           typename std::iterator_traits<Itr>::value_type,
                           typename std::iterator_traits<Itr>::difference_type,
                           typename std::iterator_traits<Itr>::pointer,
                           typename std::iterator_traits<Itr>::reference> {

public:
  filtering_iterator() = default;
  explicit filtering_iterator(Itr current, Itr end, Filter filter)
    : Filter(std::move(filter))
    , current_(std::move(current))
    , end_(std::move(end)) {}

  bool equal(filtering_iterator const& other) const noexcept {
    return current_ == other.current_;
  }

  void increment() noexcept {
    IDLE_ASSERT(current_ != end_);
    ++current_;
    advance_filtering(current_, end_, *static_cast<Filter*>(this));
  }

  typename std::iterator_traits<Itr>::reference dereference() const noexcept {
    return *current_;
  }

private:
  Itr current_;
  Itr end_;
};

template <typename Itr, typename T>
auto make_filtering_iterator(Itr begin, Itr end, T&& filter) {
  return filtering_iterator<unrefcv_t<Itr>, unrefcv_t<T>>(
      std::move(begin), std::move(end), std::forward<T>(filter));
}

template <typename Filter>
struct filter_adapter : public Filter {
  explicit filter_adapter(Filter filter)
    : Filter(std::move(filter)) {}

  template <typename Container>
  auto operator()(Container&& container) && {
    Filter& filter = *static_cast<Filter*>(this);

    // Advance the iterator forward until the first match
    auto itr = container.begin();
    auto const end = container.end();
    for (; itr != end; ++itr) {
      if (filter(*itr)) {
        break;
      }
    }

    auto fbegin = detail::make_filtering_iterator(itr, end, filter);
    auto fend = detail::make_filtering_iterator(end, end,
                                                std::forward<Filter>(
                                                    std::move(filter)));

    return Range<unrefcv_t<decltype(fbegin)>, void>(std::move(fbegin),
                                                    std::move(fend));
  }
};
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_VIEWS_FILTER_IMPL_HPP_INCLUDED
