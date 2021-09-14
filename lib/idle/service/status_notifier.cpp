
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

#include <fmt/format.h>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/parts/listener.hpp>
#include <idle/core/support.hpp>
#include <idle/interface/log.hpp>
#include <idle/interface/logger.hpp>
#include <idle/service/status_notifier.hpp>

namespace idle {
class DefaultStatusNotifier : public Extends<StatusNotifier, Listener> {
public:
  explicit DefaultStatusNotifier(Inheritance parent)
    : Extends<StatusNotifier, Listener>(std::move(parent)) {

    enable(BitSet<Event>::all());
  }

  void onServiceInit(Service& current) noexcept override {
    IDLE_LOG_TRACE(log_, "Initialized service '{}' ", current);
    (void)current;
  }
  void onServiceDestroy(Service& current) noexcept override {
    IDLE_LOG_TRACE(log_, "Destroying service '{}' ", current);
    (void)current;
  }
  void onServiceChanged(Service& current, Phase from,
                        Phase to) noexcept override {
    IDLE_LOG_TRACE(log_, "Changed service '{}' from {} to {}", current, from,
                   to);

    (void)current;
    (void)from;
    (void)to;
  }
  void onUsageConnected(Usage& current) noexcept override {
    IDLE_LOG_TRACE(log_, "Connected usage '{}' ", current);
    (void)current;
  }
  void onUsageDisconnect(Usage& current) noexcept override {
    IDLE_LOG_TRACE(log_, "Disconnecting usage '{}' ", current);
    (void)current;
  }

private:
  Dependency<Log> log_{*this};

  IDLE_SERVICE
};

Ref<StatusNotifier> StatusNotifier::create(Inheritance parent) {
  return spawn<DefaultStatusNotifier>(std::move(parent));
}
} // namespace idle
