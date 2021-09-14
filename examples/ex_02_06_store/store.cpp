
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

#include <idle/idle.hpp>

using namespace idle;

namespace examples {

/// The persisted state
struct PersistedState {
  int counter{0};
};

IDLE_REFLECT(PersistedState, counter)

/// A Service that persists its state across multiple start/stops and instances.
class PersistedService final : public Implements<Autostarted> {
public:
  using Super::Super;

  /// Get the PersistedState from the Store
  continuable<> onStart() override {
    return async([this] {
      id_ = store_->get(*this, state_);
    });
  }

  /// Update the PersistedState on the Store
  continuable<> increment() {
    return root().event_loop().async_post(
        wrap(*this, [](Use<PersistedService>&& me) {
          ++me->state_.counter;
          me->store_->update(me->id_, me->state_);
        }));
  }

  /// Persist the PersistedState on the Store
  continuable<> onStop() override {
    return async([this] {
      store_->set(std::move(id_), state_);
    });
  }

private:
  PersistedState state_;
  Store::ID id_;

  Dependency<Store> store_{*this};

  IDLE_SERVICE
};

IDLE_DECLARE(PersistedService)
} // namespace examples
