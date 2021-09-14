
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

#ifndef IDLE_CORE_PARTS_LISTENER_HPP_INCLUDED
#define IDLE_CORE_PARTS_LISTENER_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/bitset.hpp>

namespace idle {
class IDLE_API(idle) Listener : public Interface {
  friend class ContextImpl;

public:
  enum class Event : std::uint8_t {
    OnServiceInit,
    OnServiceDestroy,
    OnServiceChanged,
    OnUsageConnected,
    OnUsageDisconnected,
  };

  explicit Listener(Service& owner,
                    BitSet<Event> filter = BitSet<Event>::none());

protected:
  void enable(BitSet<Event> filter) noexcept {
    filter_ = filter;
  }

  virtual void onServiceInit(Service& current) noexcept {
    (void)current;
    IDLE_ASSERT(detail::is_on_event_loop(owner()));
  }

  virtual void onServiceDestroy(Service& current) noexcept {
    (void)current;
    IDLE_ASSERT(detail::is_on_event_loop(owner()));
  }

  virtual void onServiceChanged(Service& current, Service::Phase from,
                                Service::Phase to) noexcept {
    (void)current;
    (void)from;
    (void)to;
    IDLE_ASSERT(detail::is_on_event_loop(owner()));
  }

  virtual void onUsageDisconnect(Usage& current) noexcept {
    (void)current;
    IDLE_ASSERT(detail::is_on_event_loop(owner()));
  }

  virtual void onUsageConnected(Usage& current) noexcept {
    (void)current;
    IDLE_ASSERT(detail::is_on_event_loop(owner()));
  }

private:
  BitSet<Event> filter_;

  IDLE_INTERFACE
};
} // namespace idle

#endif // IDLE_CORE_PARTS_LISTENER_HPP_INCLUDED
