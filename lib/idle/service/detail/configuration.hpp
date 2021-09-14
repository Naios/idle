
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

#ifndef IDLE_SERVICE_DETAIL_CONFIGURATION_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_CONFIGURATION_HPP_INCLUDED

#include <boost/asio/defer.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/post.hpp>
#include <idle/core/ref.hpp>

namespace idle {
template <typename Executor>
auto through_defer(Executor& executor) {
  return [executor = std::addressof(executor)](auto&& work) {
    boost::asio::defer(*executor, std::forward<decltype(work)>(work));
  };
}

template <typename Executor>
auto through_dispatch(Executor& executor) {
  return [executor = std::addressof(executor)](auto&& work) {
    boost::asio::dispatch(*executor, std::forward<decltype(work)>(work));
  };
}

template <typename Executor>
auto through_post(Executor& executor) {
  return [executor = std::addressof(executor)](auto&& work) {
    boost::asio::post(*executor, std::forward<decltype(work)>(work));
  };
}
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_CONFIGURATION_HPP_INCLUDED
