
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

#ifndef IDLE_SERVICE_DETAIL_SIGNAL_SET_SIGNAL_SET_IMPL_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_SIGNAL_SET_SIGNAL_SET_IMPL_HPP_INCLUDED

#include <csignal>
#include <vector>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/signal_set.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/io_context.hpp>
#include <idle/service/signal_set.hpp>

namespace idle {
class signal_set_impl : public SignalSet, public Upcastable<signal_set_impl> {

public:
  explicit signal_set_impl(Inheritance parent)
    : SignalSet(std::move(parent)) {}

  continuable<signal_t> wait_impl();

  void setup_impl(Config config);

  continuable<> onStart() override;
  continuable<> onStop() override;

private:
  Dependency<IOContext> io_context_{*this};
  optional<boost::asio::signal_set> signal_set_;
  Config config_;
};
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_SIGNAL_SET_SIGNAL_SET_IMPL_HPP_INCLUDED
