
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

#ifndef IDLE_CORE_DETAIL_VIEWS_TRANSFORM_IMPL_HPP_INCLUDED
#define IDLE_CORE_DETAIL_VIEWS_TRANSFORM_IMPL_HPP_INCLUDED

#include <cstddef>
#include <type_traits>
#include <utility>
#include <idle/core/util/iterator_facade.hpp>
#include <idle/core/util/meta.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
namespace detail {
template <typename Itr, typename Mapper,
          typename Mapped = decltype(std::declval<Mapper>()(
              *std::declval<Itr>()))>
class transform_iterator
  : public Mapper,
    public iterator_facade<
        transform_iterator<Itr, Mapper>, std::forward_iterator_tag, Mapped,
        std::ptrdiff_t, std::remove_reference_t<std::remove_pointer_t<Mapped>>*,
        Mapped> {

public:
  transform_iterator() = default;
  explicit transform_iterator(Itr current, Mapper mapper)
    : Mapper(std::move(mapper))
    , current_(std::move(current)) {}

  bool equal(transform_iterator const& other) const noexcept {
    return current_ == other.current_;
  }

  void increment() noexcept {
    ++current_;
  }

  Mapped dereference() const noexcept {
    return (*static_cast<Mapper const*>(this))(*current_);
  }

private:
  Itr current_;
};

template <typename Itr, typename T>
auto make_mapping_iterator(Itr begin, Itr /*end*/, T&& mapper) {
  return transform_iterator<unrefcv_t<Itr>, unrefcv_t<T>>(begin,
                                                          std::forward<T>(
                                                              mapper));
}

template <typename Transformer>
struct transform_adapter : public Transformer {
  explicit transform_adapter(Transformer filter)
    : Transformer(std::move(filter)) {}

  template <typename Container>
  auto operator()(Container&& container) && {
    Transformer& transformer = *static_cast<Transformer*>(this);

    auto const first = container.begin();
    auto const second = container.end();

    auto begin = detail::make_mapping_iterator(first, second, transformer);
    auto end = detail::make_mapping_iterator(second, second,
                                             std::move(transformer));

    return Range<unrefcv_t<decltype(begin)>, void>(std::move(begin),
                                                   std::move(end));
  }
};
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_VIEWS_TRANSFORM_IMPL_HPP_INCLUDED
