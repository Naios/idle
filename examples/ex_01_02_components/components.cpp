
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

#include <cstdint>
#include <string>
#include <idle/idle.hpp>

using namespace idle;

namespace examples {
struct UserRecord {
  std::uint32_t id;
  std::string name;
};

/// Simulates a Service to access a database
class DataBaseConnectionService final : public Implements<> {
public:
  using Super::Super;

  struct Config {
    std::string url;
    std::string user;
    std::string password;
  };

  void setup(Config config) {
    config_ = std::move(config);
  }

  continuable<> onStart() override {
    return async([this] {
      // Initialize the connection to the server
    });
  }

  continuable<UserRecord> query(std::uint32_t id) {
    // Perform the database query
    return make_ready_continuable(UserRecord{id, "test"});
  }

  continuable<> onStop() override {
    return async([this] {
      // Disconnect the connection to the server
    });
  }

private:
  Config config_;

  IDLE_SERVICE
};

/// Implements a Service that uses lower-level functionality
/// from a lower-level DataBaseConnectionService component.
///
/// While Components are configured down from the parent to its children,
/// they are started in the reverse order: children first, then its parent.
/// This is why we are allowed to use a Component when the parent is started,
/// but not when the parent is configured.
/// On the other hand, the parent configures its children at onSetup.
class HighLevelService final : public Implements<Autostarted> {
public:
  using Super::Super;

  void onSetup() override {
    DataBaseConnectionService::Config config;
    config.url = "127.0.0.1:3306";
    config.user = "admin";
    config.password = "admin";
    db_->setup(std::move(config));
  }

  continuable<> onStart() override {
    return db_->query(1).then([](UserRecord record) {
      // Do something with the record
      (void)record.id;
      (void)record.name;
    });
  }

private:
  Component<DataBaseConnectionService> db_{*this};

  IDLE_SERVICE
};

IDLE_DECLARE(HighLevelService)
} // namespace examples
