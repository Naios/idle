
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
/// Implements an abstract Interface that can be implemented by a Service.
///
/// If we want to depend on ExampleInterface, we explicitly have to instantiate
/// a Service, that implements ExampleInterface!
class ExampleInterface : public Interface {
public:
  using Super::Super;

  virtual int implementMe() = 0;

  IDLE_INTERFACE
};

/// A specific Service that implements ExampleInterface.
///
/// The service is explicitly instantiated inside the system
/// through IDLE_DECLARE. If this Service is not present in the system,
/// the DependingService below won't start due to an unresolved dependency.
class ExampleInterfaceImplementation final
  : public Implements<ExampleInterface> {

public:
  using Super::Super;

  int implementMe() override {
    return 1;
  }
};

IDLE_DECLARE(ExampleInterfaceImplementation)

/// Implements an interface that has a default implementation available.
///
/// The default implementation will explicitly instantiated if no other Service
/// is present that provides the Interface.
class DefaultCreatableInterface : public Interface {
public:
  using Super::Super;

  virtual int implementSomething() = 0;

  static Ref<DefaultCreatableInterface> create(Inheritance parent) {
    class DefaultCreatableService final
      : public Implements<DefaultCreatableInterface> {

    public:
      using Implements<DefaultCreatableInterface>::Implements;

      int implementSomething() override {
        // Do something
        return 2;
      }

      IDLE_SERVICE
    };

    return spawn<DefaultCreatableService>(std::move(parent));
  }

  IDLE_INTERFACE
};

class DependingService final : public Implements<Autostarted> {
public:
  using Super::Super;

  continuable<> onStart() override {
    return async([this] {
      IDLE_LOG_INFO(log_, "example_->implementMe() = {}",
                    example_->implementMe());

      IDLE_LOG_INFO(log_, "default_created_->implementSomething() = {}",
                    default_created_->implementSomething());
    });
  }

private:
  Dependency<ExampleInterface> example_{*this};
  Dependency<DefaultCreatableInterface> default_created_{*this};
  Dependency<Log> log_{*this};
};

IDLE_DECLARE(DependingService)
} // namespace examples
