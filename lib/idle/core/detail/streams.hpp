
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

#ifndef IDLE_CORE_DETAIL_STREAMS_HPP_INCLUDED
#define IDLE_CORE_DETAIL_STREAMS_HPP_INCLUDED

#include <cctype>
#include <cstddef>
#include <ostream>
#include <streambuf>
#include <type_traits>
#include <utility>

namespace idle {
namespace detail {
namespace transforms {
struct alnum_fn {
  std::streambuf::int_type operator()(char value,
                                      std::streambuf& stream) const noexcept;
};

struct indentation_fn {
  std::streambuf::int_type operator()(char value,
                                      std::streambuf& stream) noexcept;

  std::streamsize indentation{2};
  bool after_newline{false};
};

struct guideline_fn {
  std::streambuf::int_type operator()(char value,
                                      std::streambuf& stream) noexcept;

  std::streamsize guideline{80};
  bool break_only_after_spaces{false};
  std::streamsize current{0};
};

struct comma_break_fn {
  std::streambuf::int_type operator()(char value,
                                      std::streambuf& stream) noexcept;

  std::streamsize max_width{12};
  std::streamsize cur_width{0};
  bool after_comma_{false};
};
} // namespace transforms

template <typename T>
class transform_stream : public std::ostream {
  class buffer : private T, public std::streambuf {
  public:
    explicit buffer(std::ostream& forward, T&& transform)
      : T(std::move(transform))
      , forward_(forward.rdbuf()) {}

    std::streambuf::int_type overflow(std::streambuf::int_type value) override {
      if (!traits_type::eq_int_type(value, traits_type::eof())) {
        return (*this)(char(value), *forward_);
      } else {
        return traits_type::not_eof(value);
      }
    }

    std::streambuf::int_type sync() override {
      std::streambuf::int_type result = this->overflow(traits_type::eof());
      forward_->pubsync();
      return result;
    }

  private:
    std::streambuf* forward_;
  };

public:
  explicit transform_stream(std::ostream& forward, T transform = T{})
    : std::ostream(&buffer_)
    , buffer_(forward, std::move(transform)) {}

  transform_stream(transform_stream&&) = default;
  transform_stream(transform_stream const&) = delete;
  transform_stream& operator=(transform_stream&&) = default;
  transform_stream& operator=(transform_stream const&) = delete;
  ~transform_stream() = default;

private:
  buffer buffer_;
};

/// Converts "18r73%hgdg/" into "18r73_hgdg_"
using alnum_stream = transform_stream<transforms::alnum_fn>;
/// Idents with 4 spaces after every newline
using indentation_stream = transform_stream<transforms::indentation_fn>;
/// Creates a newline after n character (default = 80)
using guidline_stream = transform_stream<transforms::guideline_fn>;
/// Breaks ", " into ",\n" after n characters
using comma_break_stream = transform_stream<transforms::comma_break_fn>;
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_STREAMS_HPP_INCLUDED
