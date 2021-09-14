
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

#include <exception>
#include <memory>
#include <sstream>
#include <string>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <idle/core/context.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/text.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/service/detail/default_paths.hpp>
#include <idle/service/external/toml11/serialize.hpp>
#include <idle/service/store.hpp>
#include <sqlite3/sqlite3.h>

namespace idle {

static char const* str_or_null(std::string const& str) noexcept {
  if (str.empty()) {
    return nullptr;
  } else {
    return str.c_str();
  }
}

using SQLite3DB = std::unique_ptr<sqlite3, int (*)(sqlite3*)>;
using SQLite3Statement = std::unique_ptr<sqlite3_stmt, int (*)(sqlite3_stmt*)>;

class SQLite3QueryScope {
  using Guard = std::unique_ptr<sqlite3_stmt, int (*)(sqlite3_stmt*)>;

public:
  explicit SQLite3QueryScope(sqlite3_stmt* stmt) noexcept
    : guard_(stmt, &sqlite3_reset) {}

  SQLite3QueryScope(SQLite3QueryScope const&) = delete;
  SQLite3QueryScope(SQLite3QueryScope&&) = default;
  SQLite3QueryScope& operator=(SQLite3QueryScope const&) = delete;
  SQLite3QueryScope& operator=(SQLite3QueryScope&&) = default;

private:
  Guard guard_;
};

namespace query {
static constexpr auto create = R"(
CREATE TABLE IF NOT EXISTS `idle_keys` (
  `key` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  `name` TEXT UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS `idle_store` (
  `id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  `key` INTEGER NOT NULL,
  `checksum` INTEGER NOT NULL DEFAULT 0,
  `data` BLOB NULL DEFAULT NULL,
  `expiry` DATETIME NULL DEFAULT NULL,
  `temp` BOOLEAN NOT NULL DEFAULT 0 CHECK (`temp` IN (0, 1)),
  FOREIGN KEY (`key`) REFERENCES `idle_keys` (`key`) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS `idle_locked` (
  `id` INTEGER PRIMARY KEY NOT NULL,
  FOREIGN KEY (`id`) REFERENCES `idle_store` (`id`) ON UPDATE CASCADE ON DELETE CASCADE
);

DELETE FROM `idle_locked`;

PRAGMA foreign_keys = ON;
)"_txt;

static constexpr auto create_key_with_name = R"(
INSERT INTO `idle_keys` (`name`) VALUES (?) ON CONFLICT(`name`) DO UPDATE SET name = name
)"_txt;

static constexpr auto select_key_with_name = R"(
SELECT `key` FROM `idle_keys` WHERE `name` = ?
)"_txt;

static constexpr auto lock_id = R"(
INSERT INTO `idle_locked` (`id`)
  SELECT `idle_store`.`id` FROM `idle_store`
    LEFT OUTER JOIN `idle_locked` ON `idle_store`.`id` = `idle_locked`.`id`
  WHERE `idle_locked`.`id` IS NULL
    AND `idle_store`.`key` = ?
  ORDER BY `idle_store`.`id` ASC LIMIT 1
)"_txt;

static constexpr auto unlock_id = R"(
DELETE FROM `idle_locked` WHERE `id` = ?
)"_txt;

static constexpr auto select_data_by_id = R"(
SELECT `data` FROM `idle_store` WHERE `id` = ?
)"_txt;

static constexpr auto insert_store_by_key = R"(
INSERT INTO `idle_store` (`key`) VALUES (?)
)"_txt;

static constexpr auto update_store_by_id = R"(
UPDATE `idle_store` SET `data` = ? WHERE `id` = ?
)"_txt;

static constexpr auto finalize_store_by_id = R"(
UPDATE `idle_store` SET `data` = ? WHERE `id` = ?
)"_txt;
} // namespace query

class StorageException final : public std::exception {
public:
  explicit StorageException(std::string msg)
    : msg_(std::move(msg)) {}

  char const* what() const noexcept override {
    return msg_.c_str();
  }

private:
  std::string msg_;
};

static int sqlite_try(sqlite3& db, int result) noexcept(false) {
  switch (result) {
    case SQLITE_OK:
    case SQLITE_ROW:
    case SQLITE_DONE:
      return result;
    default: {
      throw StorageException(fmt::format(FMT_STRING("Sqlite3 error {}: '{}'!"),
                                         sqlite3_errcode(&db),
                                         sqlite3_errmsg(&db)));
    }
  }
}

static SQLite3Statement sqlite_prepare(sqlite3& db,
                                       StringView query) noexcept(false) {
  sqlite3_stmt* stmt;
  sqlite_try(db, sqlite3_prepare_v2(&db, query.data(), query.size(), &stmt,
                                    nullptr));
  return SQLite3Statement(stmt, &sqlite3_finalize);
}

static void sqlite_execute(sqlite3& db, StringView query) {
  auto const stmt = sqlite_prepare(db, query);

  sqlite_try(db, sqlite3_step(stmt.get()));
}
static void sqlite_batch_execute(sqlite3& db, StringView query) {
  while (StringView part = query.split(';')) {
    sqlite_execute(db, part);
  }
}

class DefaultStoreImpl final : public Extends<DefaultStore>,
                               public Upcastable<DefaultStoreImpl> {
  friend DefaultStore;

  using KeyID = std::uint64_t;

  struct Env {
    SQLite3DB db{nullptr, nullptr};
    SQLite3Statement create_key_with_name{nullptr, nullptr};
    SQLite3Statement select_key_with_name{nullptr, nullptr};
    SQLite3Statement lock_id{nullptr, nullptr};
    SQLite3Statement unlock_id{nullptr, nullptr};
    SQLite3Statement select_data_by_id{nullptr, nullptr};
    SQLite3Statement insert_store_by_key{nullptr, nullptr};
    SQLite3Statement update_store_by_id{nullptr, nullptr};
    SQLite3Statement finalize_store_by_id{nullptr, nullptr};
  };

public:
  using Extends<DefaultStore>::Extends;

  continuable<> onStart() override {
    return async([this] {
      IDLE_ASSERT(!env_.db);

      IDLE_DETAIL_LOG_DEBUG("Opening an SQLite3 storage at '{}'",
                            config_.path.empty() ? "<memory>"
                                                 : config_.path.c_str());

      if (sqlite3_initialize() != SQLITE_OK) {
        throw StorageException("Failed to initialize sqlite3!");
      }

      Env env;

      env.db = [&] {
        sqlite3* init;

        if (sqlite3_open(str_or_null(config_.path), &init) != SQLITE_OK) {
          if (config_.path.empty()) {
            throw StorageException("Failed to create a sqlite3 db in memory!");
          } else {
            throw StorageException(fmt::format(
                FMT_STRING("Failed to open the sqlite3 db at '{}'!"),
                config_.path));
          }
        }

        return SQLite3DB(init, &sqlite3_close);
      }();

      sqlite_batch_execute(*env.db, query::create);
      env.create_key_with_name = sqlite_prepare(*env.db,
                                                query::create_key_with_name);
      env.select_key_with_name = sqlite_prepare(*env.db,
                                                query::select_key_with_name);
      env.lock_id = sqlite_prepare(*env.db, query::lock_id);
      env.unlock_id = sqlite_prepare(*env.db, query::unlock_id);
      env.select_data_by_id = sqlite_prepare(*env.db, query::select_data_by_id);
      env.insert_store_by_key = sqlite_prepare(*env.db,
                                               query::insert_store_by_key);
      env.update_store_by_id = sqlite_prepare(*env.db,
                                              query::update_store_by_id);
      env.finalize_store_by_id = sqlite_prepare(*env.db,
                                                query::finalize_store_by_id);
      env_ = std::move(env);
    });
  }

  continuable<> onStop() override {
    return async([this] {
      env_ = {};
    });
  }

  KeyID create_key_with_name(StringView key) const {
    IDLE_ASSERT(env_.create_key_with_name);
    IDLE_ASSERT(env_.select_key_with_name);

    {
      sqlite3_stmt* stmt = env_.create_key_with_name.get();
      SQLite3QueryScope const scope(stmt);
      sqlite3_bind_text(stmt, 1, key.data(), key.size(), nullptr);

      IDLE_DETAIL_LOG_TRACE("Create name query: '{}'", sqlite3_sql(stmt));
      IDLE_CHECK(sqlite_try(*env_.db, sqlite3_step(stmt)) == SQLITE_DONE);

      IDLE_ASSERT(sqlite3_changes(env_.db.get()) == 1);
    }

    // sqlite3_last_insert_rowid does not represent the inserted row
    sqlite3_stmt* stmt = env_.select_key_with_name.get();
    SQLite3QueryScope const scope(stmt);
    sqlite3_bind_text(stmt, 1, key.data(), key.size(), nullptr);

    IDLE_DETAIL_LOG_TRACE("Select name query: '{}'", sqlite3_sql(stmt));
    IDLE_CHECK(sqlite_try(*env_.db, sqlite3_step(stmt)) == SQLITE_ROW);

    KeyID const key_id = sqlite3_column_int64(stmt, 0);
    IDLE_ASSERT(key_id);
    return key_id;
  }

  /// Tries to lock an id with the specific key
  ID try_lock(KeyID key) const {
    IDLE_ASSERT(env_.lock_id);

    sqlite3_stmt* stmt = env_.lock_id.get();
    SQLite3QueryScope const lock_scope(stmt);
    sqlite3_bind_int64(stmt, 1, key);

    IDLE_DETAIL_LOG_TRACE("Lock query: '{}'", sqlite3_sql(stmt));
    IDLE_CHECK(sqlite_try(*env_.db, sqlite3_step(stmt)) == SQLITE_DONE);

    auto const changes = sqlite3_changes(env_.db.get());
    if (changes) {
      IDLE_ASSERT(changes == 1);
      return ID{
          static_cast<ID::type>(sqlite3_last_insert_rowid(env_.db.get()))};
    } else {
      return ID{};
    }
  }

  ID acquire_locked(KeyID key) const {
    for (;;) {
      if (auto id = try_lock(key)) {
        return id;
      }

      // Insert an empty storage row
      sqlite3_stmt* stmt = env_.insert_store_by_key.get();
      SQLite3QueryScope const scope(stmt);

      sqlite3_bind_int64(stmt, 1, key);

      IDLE_DETAIL_LOG_TRACE("Create store query: '{}'", sqlite3_sql(stmt));
      IDLE_CHECK(sqlite_try(*env_.db, sqlite3_step(stmt)) == SQLITE_DONE);

      IDLE_ASSERT(sqlite3_last_insert_rowid(env_.db.get()));
    }
  }

  ID get(StringView key, Consumer callback) override {
    IDLE_ASSERT(key);
    IDLE_ASSERT(root().is_on_event_loop());

    KeyID const keyid = create_key_with_name(key);
    ID id(acquire_locked(keyid));

    sqlite3_stmt* stmt = env_.select_data_by_id.get();
    SQLite3QueryScope const scope(stmt);

    sqlite3_bind_int64(stmt, 1, *id);

    IDLE_DETAIL_LOG_TRACE("Select query: '{}'", sqlite3_sql(stmt));
    IDLE_CHECK(sqlite_try(*env_.db, sqlite3_step(stmt)) == SQLITE_ROW);

    auto const type = sqlite3_column_type(stmt, 0);
    if (type != SQLITE_NULL) {
      auto const data = sqlite3_column_blob(stmt, 0);
      auto const size = sqlite3_column_bytes(stmt, 0);

      BufferView const buffer{static_cast<BufferView::pointer>(data),
                              static_cast<std::size_t>(size)};
      callback(buffer);
    }

    return id;
  }

  void update(ID const& id, BufferView buffer) override {
    IDLE_ASSERT(id);
    IDLE_ASSERT(root().is_on_event_loop());

    sqlite3_stmt* stmt = env_.update_store_by_id.get();
    SQLite3QueryScope const scope(stmt);

    sqlite3_bind_blob(stmt, 1, buffer.data(), buffer.size(), nullptr);
    sqlite3_bind_int64(stmt, 2, static_cast<std::int64_t>(*id));

    IDLE_DETAIL_LOG_TRACE("Update store query: '{}'", sqlite3_sql(stmt));
    IDLE_CHECK(sqlite_try(*env_.db, sqlite3_step(stmt)) == SQLITE_DONE);
  }

  void set(ID&& id, BufferView buffer) override {
    IDLE_ASSERT(id);
    IDLE_ASSERT(root().is_on_event_loop());

    ID const local = std::move(id);

    {
      sqlite3_stmt* stmt = env_.finalize_store_by_id.get();
      SQLite3QueryScope const scope(stmt);

      sqlite3_bind_blob(stmt, 1, buffer.data(), buffer.size(), nullptr);
      sqlite3_bind_int64(stmt, 2, static_cast<std::int64_t>(*local));

      IDLE_DETAIL_LOG_TRACE("Finalize store query: '{}'", sqlite3_sql(stmt));
      IDLE_CHECK(sqlite_try(*env_.db, sqlite3_step(stmt)) == SQLITE_DONE);
    }

    {
      sqlite3_stmt* stmt = env_.unlock_id.get();
      SQLite3QueryScope const scope(stmt);

      sqlite3_bind_int64(stmt, 1, static_cast<std::int64_t>(*local));

      IDLE_DETAIL_LOG_TRACE("Unlock store query: '{}'", sqlite3_sql(stmt));
      IDLE_CHECK(sqlite_try(*env_.db, sqlite3_step(stmt)) == SQLITE_DONE);
    }
  }

private:
  Config config_{defaultLocation()};
  Env env_;
};

Store::ID Store::get(StringView key, ReflectionPtr out) {
  return get(key, [=](BufferView buffer) {
    using namespace boost::iostreams;

    basic_array_source<char> source(buffer.data(), buffer.size());
    stream<basic_array_source<char>> stream(source);

    auto const data = toml::parse(stream);
    toml_deserialize(data, out);
  });
}

static std::string blob_serialize(ConstReflectionPtr in) {
  // TODO Use a more efficient binary serialization here
  // for now, serialization and deserialization to and from toml
  // should work fine.
  toml::basic_value<toml::preserve_comments> toml;
  toml_serialize(toml, in);
  std::ostringstream ss;
  ss << toml;
  return ss.str();
}

void Store::update(ID const& id, ConstReflectionPtr in) {
  update(id, blob_serialize(in));
}

void Store::set(ID&& id, ConstReflectionPtr in) {
  set(std::move(id), blob_serialize(in));
}

Ref<Store> Store::create(Inheritance parent) {
  return DefaultStore::create(std::move(parent));
}

std::string DefaultStore::defaultLocation() {
  return detail::get_default_path("db").generic_string();
}

void DefaultStore::setup(Config config) {
  DefaultStoreImpl::from(this)->config_ = std::move(config);
}

Ref<DefaultStore> DefaultStore::create(Inheritance parent) {
  return spawn<DefaultStoreImpl>(std::move(parent));
}
} // namespace idle
