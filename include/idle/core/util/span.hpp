
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

#ifndef IDLE_CORE_UTIL_SPAN_HPP_INCLUDED
#define IDLE_CORE_UTIL_SPAN_HPP_INCLUDED

#include <type_traits>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/iterator_facade.hpp>

namespace idle {
static constexpr std::size_t dynamic_extent = static_cast<std::size_t>(-1);

template <typename T>
class Span {
public:
  using size_type = std::size_t;
  using value_type = std::remove_const_t<T>;
  using pointer = std::add_pointer_t<T>;
  using const_pointer = std::add_pointer_t<std::add_const_t<T>>;
  using reference = std::add_lvalue_reference_t<T>;
  using const_reference = std::add_lvalue_reference_t<std::add_const_t<T>>;
  using difference_type = std::ptrdiff_t;

  static constexpr size_type npos = dynamic_extent;

  class iterator
    : public iterator_facade<iterator, std::random_access_iterator_tag,
                             value_type, difference_type, pointer, reference> {

  public:
    constexpr iterator() noexcept {}

    explicit constexpr iterator(pointer current)
      : current_(current) {}

    constexpr bool equal(iterator const& other) const noexcept {
      return current_ == other.current_;
    }

    constexpr void increment() noexcept {
      ++current_;
    }
    constexpr void decrement() noexcept {
      --current_;
    }
    constexpr void advance(difference_type n) noexcept {
      current_ += n;
    }

    constexpr reference dereference() const noexcept {
      return *current_;
    }

    constexpr difference_type
    distance_to(iterator const& right) const noexcept {
      return right.current_ - current_;
    }

  private:
    pointer current_{nullptr};
  };

  class const_iterator
    : public iterator_facade<const_iterator, std::random_access_iterator_tag,
                             value_type, difference_type, const_pointer,
                             const_reference> {

  public:
    constexpr const_iterator() noexcept {}

    explicit constexpr const_iterator(const_pointer current)
      : current_(current) {}

    constexpr bool equal(const_iterator const& other) const noexcept {
      return current_ == other.current_;
    }

    constexpr void increment() noexcept {
      ++current_;
    }
    constexpr void decrement() noexcept {
      --current_;
    }
    constexpr void advance(difference_type n) noexcept {
      current_ += n;
    }

    constexpr const_reference dereference() const noexcept {
      return *current_;
    }

    constexpr difference_type
    distance_to(const_iterator const& right) const noexcept {
      return right.current_ - current_;
    }

  private:
    const_pointer current_{nullptr};
  };

  constexpr Span() noexcept
    : Span(nullptr, 0) {
    // Also a clang compiler bug workaround for empty default constructors
    // https://stackoverflow.com/questions/43819314/default-member-initializer-needed-within-definition-of-enclosing-class-outside
  }
  template <std::size_t Size>
  /* implicit */ constexpr Span(T (&data)[Size]) noexcept
    : Span(data, Size) {}
  constexpr Span(pointer data, size_type size) noexcept
    : data_(data)
    , size_(size) {}

  template <typename C,
            decltype(const_pointer(std::declval<C>().data()))* = nullptr,
            decltype(size_type(std::declval<C>().size()))* = nullptr>
  /* implicit */ constexpr Span(C&& src) noexcept(
      noexcept(std::declval<C>().data()) && noexcept(std::declval<C>().size()))
    : Span(src.data(), src.size()) {}

  constexpr iterator begin() noexcept {
    return iterator{data_};
  }
  constexpr iterator end() noexcept {
    return iterator{data_ + size()};
  }
  constexpr const_iterator begin() const noexcept {
    return const_iterator{data_};
  }
  constexpr const_iterator end() const noexcept {
    return const_iterator{data_ + size()};
  }
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator{data_};
  }
  constexpr const_iterator cend() const noexcept {
    return const_iterator{data_ + size()};
  }

  constexpr bool empty() const noexcept {
    return size_ == 0U;
  }
  constexpr explicit operator bool() const noexcept {
    return !empty();
  }

  constexpr void clear() noexcept {
    data_ = nullptr;
    size_ = 0;
  }

  constexpr reference operator[](size_type idx) const noexcept {
    return data_[idx];
  }

  constexpr pointer data() const noexcept {
    return data_;
  }
  constexpr size_type size() const noexcept {
    return size_;
  }

  constexpr Span subspan(size_type pos = 0, //
                         size_type count = dynamic_extent) const noexcept {
    if (count == dynamic_extent) {
      return Span(data_ + pos, size_ - pos);
    } else {
      return Span(data_ + pos, count);
    }
  }

  constexpr size_type find(T value, size_type pos = 0U) const noexcept {
    for (; pos < size(); ++pos) {
      if (operator[](pos) == value) {
        return pos;
      }
    }
    return npos;
  }

  constexpr size_type rfind(T value, size_type pos = npos) const noexcept {
    if (pos >= size()) {
      pos = size() - 1;
    }

    for (; pos > 0; --pos) {
      if (operator[](pos) == value) {
        return pos;
      }
    }
    return npos;
  }

  constexpr Span split(T value) noexcept {
    size_type const next = find(value);

    if (next != npos) {
      Span current = subspan(0, next);
      *this = subspan(next + 1);
      return current;
    } else {
      Span current = *this;
      clear();
      return current;
    }
  }

  constexpr bool operator==(Span other) const noexcept {
    std::size_t const my_size = size();

    if (my_size == other.size()) {
      // constexpr equality
      for (size_type i = 0; i != my_size; ++i) {
        if ((*this)[i] != other[i]) {
          return false;
        }
      }
      return true;
    } else {
      return false;
    }
  }

private:
  pointer data_{};
  size_type size_{0};
};
} // namespace idle

#endif // IDLE_CORE_UTIL_SPAN_HPP_INCLUDED
