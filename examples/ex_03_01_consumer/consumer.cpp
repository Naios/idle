
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
// Make sure to also move the abstract directory into the project source dir!
#include "../ex_03_01_abstract/abstract.hpp"

using namespace idle;

namespace examples {
/// A Service that depends on all AbstractInterface,
/// but cannot change its dependencies while running.
class StaticMultiConsumerService final : public Implements<Autostarted> {
public:
  using Super::Super;

  continuable<> onStart() override {
    return async([this] {
      IDLE_LOG_INFO(log_, "StaticMultiConsumerService starting...");

      for (AbstractInterface& impl : dependencies_) {
        IDLE_LOG_INFO(log_,
                      "StaticMultiConsumerService: AbstractInterface depends "
                      "on '{}' implemented by '{}'.",
                      impl.message(), impl.owner().name());
      }
    });
  }

  continuable<> onStop() override {
    return async([this] {
      IDLE_LOG_INFO(log_, "StaticMultiConsumerService stopping...");
    });
  }

private:
  Dependency<Log> log_{*this};

  Dependency<AbstractDefaultInterface> default_{*this};

  DynDependencies<AbstractInterface, StaticMultiConsumerService,
                  VectorStorage<AbstractInterface>>
      dependencies_{*this};

  IDLE_SERVICE
};

IDLE_DECLARE(StaticMultiConsumerService)

/// A Service that depends on all AbstractInterface,
/// that is able to change its dependencies while running.
class DynMultiConsumerService final : public Implements<Autostarted> {
public:
  using Super::Super;

  continuable<> onStart() override {
    return async([this] {
      IDLE_LOG_INFO(log_, "DynMultiConsumerService starting...");

      for (AbstractInterface& impl : dependencies_) {
        IDLE_LOG_INFO(log_,
                      "DynMultiConsumerService: AbstractInterface depends "
                      "on '{}' implemented by '{}'.",
                      impl.message(), impl.owner().name());
      }
    });
  }

  continuable<> onStop() override {
    return async([this] {
      IDLE_LOG_INFO(log_, "DynMultiConsumerService stopping...");
    });
  }

  ReleaseReply onReleaseRequest(AbstractInterface& dep) noexcept {
    IDLE_LOG_INFO(log_, "DynMultiConsumerService: Removed dynamically: {}",
                  dep.owner().name());

    return ReleaseReply::Now;
  }

  UseReply onUseOffer(AbstractInterface& dep) noexcept {
    if (dep.owner().state().isRunning()) {
      IDLE_LOG_INFO(log_, "DynMultiConsumerService: Added dynamically: {}",
                    dep.owner().name());
      return UseReply::UseNowAndInsert;
    } else {
      return UseReply::UseLaterAndNotify;
    }
  }

  BitSet<DependenciesFlags> onInspect(AbstractInterface& dep) noexcept {
    (void)dep;
    return {DependenciesFlags::Weak};
  }

private:
  Dependency<Log> log_{*this};

  Component<Timer> timer_{*this};

  DynDependencies<AbstractInterface, DynMultiConsumerService,
                  VectorStorage<AbstractInterface>>
      dependencies_{*this,
                    IDLE_STATIC_BIND(
                        &DynMultiConsumerService::onReleaseRequest),
                    IDLE_STATIC_BIND(&DynMultiConsumerService::onUseOffer),
                    IDLE_STATIC_BIND(&DynMultiConsumerService::onInspect)};

  IDLE_SERVICE
};

IDLE_DECLARE(DynMultiConsumerService)
} // namespace examples
