
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

#ifndef IDLE_SERVICE_DETAIL_PROMISES_BEAST_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_PROMISES_BEAST_HPP_INCLUDED

#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/service/detail/promises_mapping.hpp>

namespace idle {
namespace beast {
template <typename Stream, typename Buffer, typename Response>
auto async_read(Stream& stream, Buffer&& buffer, Response&& response) {
  return promisify<std::size_t>::with(
      default_boost_asio_resolver(),
      [](auto&&... args) mutable {
        boost::beast::http::async_read(std::forward<decltype(args)>(args)...);
      },
      stream, std::forward<Buffer>(buffer), std::forward<Response>(response));
}

template <typename Stream, typename Request>
auto async_write(Stream& stream, Request& request) {
  return promisify<std::size_t>::with(
      default_boost_asio_resolver(),
      [&](auto&&... args) mutable {
        boost::beast::http::async_write(std::forward<decltype(args)>(args)...);
      },
      stream, request);
}
} // namespace beast
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_PROMISES_BEAST_HPP_INCLUDED
