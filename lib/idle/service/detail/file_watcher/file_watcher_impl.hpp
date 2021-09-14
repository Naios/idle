
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

#ifndef IDLE_SERVICE_DETAIL_FILE_WATCHER_FILE_WATCHER_IMPL_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_FILE_WATCHER_FILE_WATCHER_IMPL_HPP_INCLUDED

#include <atomic>
#include <chrono>
#include <unordered_map>
#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/error_code.hpp>
#include <efsw/efsw.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/util/lazy.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/io_context.hpp>
#include <idle/service/file_watcher.hpp>

namespace boost {
namespace filesystem {
class path;
}
} // namespace boost

namespace idle {
class Container;
class FileWatcherInstance;
} // namespace idle

namespace idle {
class FileWatcherInstance : public Implements<Interface> {
public:
  using Implements<Interface>::Implements;

  continuable<> onStart() override;
  continuable<> onStop() override;

  boost::asio::io_context& ioContext() noexcept {
    IDLE_ASSERT(state().isRunning());
    return io_context_->get();
  }

  static Ref<FileWatcherInstance> create(Inheritance parent);

  efsw::WatchID addWatch(std::string const& directory,
                         efsw::FileWatchListener* watcher, bool recursive);
  void removeWatch(efsw::WatchID id);

private:
  static bool should_use_legacy_watcher() noexcept;

  Dependency<IOContext> io_context_{*this};
  Lazy<efsw::FileWatcher> watcher_;

#ifndef NDEBUG
  std::atomic<std::size_t> dir_group_counter_{0};
#endif

  IDLE_SERVICE
  IDLE_INTERFACE
};

class FileWatcherImpl : public FileWatcher,
                        public Upcastable<FileWatcherImpl>,
                        public efsw::FileWatchListener {

  friend FileWatcherInstance;

  using time_point = clock_type::time_point;

public:
  using FileWatcher::FileWatcher;
  ~FileWatcherImpl() override;

  void setupImpl(Config config);

  continuable<FileChanges> watchImpl();
  continuable<FileChanges> watchImpl(duration debounce_time);

  void handleFileAction(efsw::WatchID watchid, std::string const& dir,
                        std::string const& filename, efsw::Action action,
                        std::string oldFilename) override;

protected:
  continuable<> onStart() override;
  continuable<> onStop() override;

private:
  void removeWatches();

  void debounce(duration debounce_time);

  void resolve();
  static bool has_elapsed(time_point time_point, duration duration) noexcept;

  void add_files_recursively(boost::filesystem::path const& dir,
                             bool recursive);
  void add_files_initially();

  struct PromisePair {
    promise<FileChanges> promise_;
    duration debounce_time_;
  };

  Dependency<FileWatcherInstance> instance_{*this};

  Config config_;
  bool initial_add_{false};
  Lazy<boost::asio::basic_waitable_timer<clock_type>> debounce_timer_;
  FileChanges changes_;
  time_point time_last_changed_;
  Lazy<PromisePair> promise_;
  std::vector<efsw::WatchID> watch_ids_;
};
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_FILE_WATCHER_FILE_WATCHER_IMPL_HPP_INCLUDED
