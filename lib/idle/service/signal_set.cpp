
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
#include <idle/core/ref.hpp>
#include <idle/service/detail/signal_set/signal_set_impl.hpp>
#include <idle/service/signal_set.hpp>

namespace idle {
SignalSet::Config SignalSet::Config::signal_on_break() {
  return {{SIGINT, //
           SIGTERM //
#if defined(SIGQUIT)
           ,
           SIGQUIT
#endif // SIGQUIT
  }};
}

void SignalSet::setup(Config config) {
  return signal_set_impl::from(this)->setup_impl(std::move(config));
}

continuable<SignalSet::signal_t> SignalSet::wait() {
  return signal_set_impl::from(this)->wait_impl();
}

Ref<SignalSet> SignalSet::create(Inheritance parent) {
  return spawn<signal_set_impl>(std::move(parent));
}
} // namespace idle
