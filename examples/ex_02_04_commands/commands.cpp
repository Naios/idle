
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
/// Implements an eager service that logs some messages to the global log.
class ExampleCommandService final : public Implements<Command> {
public:
  using Super::Super;

  continuable<> invoke(Ref<Session> session, Arguments&& args) override {
    return async(wrap(*this, [session, args = std::move(args)](
                                 Use<ExampleCommandService>&& me) {
      (void)me;
      session->sink().write("ExampleCommandService invoked!");
    }));
  }

  std::string command_name() const noexcept override {
    return "example test";
  }

private:
  Dependency<Log> log_{*this};

  IDLE_SERVICE
};

IDLE_DECLARE(ExampleCommandService)
} // namespace examples
