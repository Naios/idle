
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

#ifndef IDLE_SERVICE_FILE_WATCHER_HPP_INCLUDED
#define IDLE_SERVICE_FILE_WATCHER_HPP_INCLUDED

#include <chrono>
#include <iosfwd>
#include <unordered_map>
#include <vector>
#include <idle/core/api.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/dep/function.hpp>
#include <idle/core/dep/variant.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
namespace file_event {
struct FileAdded {
  // ...
};
struct FileRemoved {
  // ...
};
struct FileModified {
  // ...
};
struct FileRenamed {
  std::string old_path;
};

IDLE_API(idle)
std::ostream& operator<<(std::ostream&, FileAdded const&);
IDLE_API(idle)
std::ostream& operator<<(std::ostream&, FileRemoved const&);
IDLE_API(idle)
std::ostream& operator<<(std::ostream&, FileModified const&);
IDLE_API(idle)
std::ostream& operator<<(std::ostream&, FileRenamed const&);
} // namespace file_event

class IDLE_API(idle) FileWatcher : public Service {
  friend class FileWatcherInstance;
  friend class FileWatcherImpl;
  using Service::Service;

  using clock_type = std::chrono::steady_clock;
  using duration = clock_type::duration;

  struct NoFilterPredicate {
    bool operator()(StringView /*path*/) const {
      return true;
    }
  };

public:
  using FileOperation = variant<file_event::FileAdded, file_event::FileRemoved,
                                file_event::FileModified,
                                file_event::FileRenamed>;

  using FileChanges = std::unordered_map<std::string,    // path
                                         FileOperation>; // event

  using FIleFilterEvent = unique_function<bool(StringView /*path*/) const>;

  struct Entry {
    explicit Entry(std::string path_, bool recursive_ = true,
                   bool initial_create_ = false)
      : path(std::move(path_))
      , recursive(recursive_)
      , initial_create(initial_create_) {}

    std::string path;
    bool recursive;
    bool initial_create;
  };

  struct Config {
    std::vector<Entry> watched;
    bool initial_add{false};
    FIleFilterEvent filter = NoFilterPredicate{};
  };

  void setup(Config config);

  continuable<FileChanges> watch();
  continuable<FileChanges> watch(duration debounce_time);

  static Ref<FileWatcher> create(Inheritance parent);

  IDLE_SERVICE
};
} // namespace idle

#endif // IDLE_SERVICE_FILE_WATCHER_HPP_INCLUDED
