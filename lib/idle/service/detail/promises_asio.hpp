
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

#ifndef IDLE_SERVICE_DETAIL_PROMISES_ASIO_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_PROMISES_ASIO_HPP_INCLUDED

#include <boost/asio/basic_socket.hpp>
#include <boost/asio/basic_stream_socket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/write.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/service/detail/promises_mapping.hpp>

namespace idle {
namespace asio {
template <typename T>
auto async_resolve(boost::asio::ip::basic_resolver<T>& resolver,
                   std::string host, std::string port) {
  using results = typename boost::asio::ip::basic_resolver<T>::results_type;
  return promisify<results>::with(
      default_boost_asio_resolver(),
      [&resolver](auto&&... args) mutable {
        resolver.async_resolve(std::forward<decltype(args)>(args)...);
      },
      std::move(host), std::move(port));
}

template <typename T, typename Iterator>
auto async_connect(boost::asio::basic_socket<T>& socket, Iterator begin,
                   Iterator end) {
  return promisify<boost::asio::ip::tcp::resolver::iterator>::with(
      default_boost_asio_resolver(),
      [&socket](auto&&... args) mutable {
        boost::asio::async_connect(socket,
                                   std::forward<decltype(args)>(args)...);
      },
      std::move(begin), std::move(end));
}
template <typename T, typename Iterator>
auto async_connect(boost::asio::ssl::stream<T>& stream, Iterator begin,
                   Iterator end) {
  return async_connect(stream.next_layer(), std::move(begin), std::move(end));
}

template <typename T, typename Type>
auto async_handshake(boost::asio::basic_stream_socket<T>& /*stream*/, Type&&) {
  return make_ready_continuable();
}
template <typename T>
auto async_handshake(boost::asio::ssl::stream<T>& stream,
                     boost::asio::ssl::stream_base::handshake_type handshake) {
  return promisify<void>::with(
      default_boost_asio_resolver(),
      [&stream](auto&&... args) mutable {
        stream.async_handshake(std::forward<decltype(args)>(args)...);
      },
      std::move(handshake));
}

template <typename T, typename Buffer>
auto async_read(boost::asio::basic_socket<T>& socket, Buffer&& buffer) {
  return promisify<std::size_t>::with(
      default_boost_asio_resolver(),
      [&socket](auto&&... args) mutable {
        boost::asio::async_read(socket, std::forward<decltype(args)>(args)...);
      },
      std::forward<Buffer>(buffer));
}

template <typename T, typename Buffer>
auto async_write(boost::asio::basic_socket<T>& socket, Buffer&& buffer) {
  return promisify<std::size_t>::with(
      default_boost_asio_resolver(),
      [&socket](auto&&... args) mutable {
        boost::asio::async_write(socket, std::forward<decltype(args)>(args)...);
      },
      std::forward<Buffer>(buffer));
}

/// Shuts down the given stream synchronously
template <typename T>
auto async_shutdown(boost::asio::basic_stream_socket<T>& stream) {
  return promisify<void>::with(
      default_boost_asio_resolver(), [&stream](auto&& callback) mutable {
        boost::system::error_code error;
        stream.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
        stream.close();
        std::forward<decltype(callback)>(callback)(error);
      });
}
/// Shuts down the given SSL stream asynchronously
template <typename T>
auto async_shutdown(boost::asio::ssl::stream<T>& stream) {
  return promisify<void>::with(
      // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
      ignore_error_code_boost_resolver(boost::asio::error::eof),
      [&stream](auto&&... args) mutable {
        stream.async_shutdown(std::forward<decltype(args)>(args)...);
      });
}
} // namespace asio
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_PROMISES_ASIO_HPP_INCLUDED
