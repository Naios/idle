
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

#include <csignal>
#include <boost/asio/signal_set.hpp>
#include <continuable/external/asio.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/service/detail/signal_set/signal_set_impl.hpp>
#include <idle/service/signal_set.hpp>

namespace idle {
continuable<SignalSet::signal_t> signal_set_impl::wait_impl() {
  return signal_set_->async_wait(cti::use_continuable);
}

void signal_set_impl::setup_impl(Config config) {
  config_ = std::move(config);
}

continuable<> signal_set_impl::onStart() {
  return async([this] {
    boost::asio::io_context& context = io_context_->get();
    signal_set_.emplace(context);

    for (auto signal : config_.signals) {
      signal_set_->add(signal);
    }
  });
}

continuable<> signal_set_impl::onStop() {
  return async([this] {
    signal_set_->cancel();
    signal_set_->clear();
    signal_set_.reset();
  });
}
} // namespace idle
