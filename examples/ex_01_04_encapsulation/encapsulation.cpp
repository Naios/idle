
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
/// Implements a Service that hides its implementation
class HiddenService : public Service {
  friend HiddenService;
  using Super::Super;

public:
  continuable<> doSomething();

  static Ref<HiddenService> create(Inheritance parent);
};

/// A Service that uses the HiddenService
class UserService final : public Implements<> {
public:
  using Super::Super;

  continuable<> onStart() override {
    return hidden_->doSomething();
  }

private:
  Component<HiddenService> hidden_{*this};
};

IDLE_DECLARE(UserService)

////// The following implementations belong to a .cpp file:

// The actual implementation of HiddenService
class HiddenServiceImpl final : public Extends<HiddenService, Autostarted> {
public:
  using Super::Super;

  continuable<> doSomethingImpl() {
    (void)private_data_;
    return make_ready_continuable();
  }

private:
  int private_data_{0};
};

continuable<> HiddenService::doSomething() {
  return static_cast<HiddenServiceImpl*>(this)->doSomethingImpl();
}

Ref<HiddenService> HiddenService::create(Inheritance parent) {
  return spawn<HiddenServiceImpl>(std::move(parent));
}
} // namespace examples
