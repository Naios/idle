
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

#include <cstdlib>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/range/iterator_range.hpp>
#include <continuable/external/asio.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/use.hpp>
#include <idle/core/util/enum.hpp>
#include <idle/core/util/enum_map.hpp>
#include <idle/core/util/enumerate.hpp>
#include <idle/service/detail/file_watcher/file_watcher_impl.hpp>
#include <idle/service/file_watcher.hpp>

namespace idle {
static void convert_to_absolute_dirs(std::vector<FileWatcher::Entry>& dirs) {
  for (auto& dir : dirs) {
    boost::filesystem::path path(dir.path);
    dir.path = system_complete(std::move(path)).generic_string();
  }
}

continuable<> FileWatcherInstance::onStart() {
  return async([this] {
    IDLE_ASSERT(!watcher_);
    IDLE_ASSERT(dir_group_counter_.load() == 0);

    bool const legacy_watcher = should_use_legacy_watcher();
    watcher_.emplace(legacy_watcher);
    watcher_->watch();
  });
}

continuable<> FileWatcherInstance::onStop() {
  return async([this] {
    IDLE_ASSERT(watcher_);
    IDLE_ASSERT(dir_group_counter_ == 0);

    watcher_.reset();
  });
}

Ref<FileWatcherInstance> FileWatcherInstance::create(Inheritance parent) {
  return spawn<FileWatcherInstance>(std::move(parent));
}

efsw::WatchID FileWatcherInstance::addWatch(std::string const& directory,
                                            efsw::FileWatchListener* watcher,
                                            bool recursive) {

  IDLE_ASSERT(root().is_on_event_loop());
  IDLE_ASSERT(!directory.empty());

  efsw::WatchID const id = watcher_->addWatch(directory, watcher, recursive);

#ifndef NDEBUG
  if (id >= 0) {
    ++dir_group_counter_;
  }
#endif

  return id;
}

void FileWatcherInstance::removeWatch(efsw::WatchID id) {
  IDLE_ASSERT(id >= 0);
  IDLE_ASSERT(root().is_on_event_loop());

#ifndef NDEBUG
  --dir_group_counter_;
#endif

  watcher_->removeWatch(id);
}

bool FileWatcherInstance::should_use_legacy_watcher() noexcept {
  using namespace boost::process;
  native_environment const environment = boost::this_process::environment();
  auto const var = environment.find("IDLE_VM");
  if (var != environment.end()) {
    return true;
  } else {
    return false;
  }
}

FileWatcherImpl::~FileWatcherImpl() {
  IDLE_ASSERT(watch_ids_.empty());
}

void FileWatcherImpl::setupImpl(Config config) {
  IDLE_ASSERT(state().isConfigurable());

  config_.watched = std::move(config.watched);
  convert_to_absolute_dirs(config_.watched);

  config_.initial_add = config.initial_add;
  config_.filter = std::move(config.filter);
}

continuable<FileWatcher::FileChanges> FileWatcherImpl::watchImpl() {
  return watchImpl(std::chrono::milliseconds(100));
}

continuable<FileWatcher::FileChanges>
FileWatcherImpl::watchImpl(duration debounce_time) {
  IDLE_ASSERT(state().isRunning());

  return [handle = handleOf(*this), debounce_time](auto&& promise) mutable {
    if (auto me = handle.lock()) {
      me->root().event_loop().async_post(
          [handle = std::move(handle),
           promise = std::forward<decltype(promise)>(promise),
           debounce_time]() mutable {
            if (auto me = handle.lock()) {
              IDLE_ASSERT(me->state().isRunning());

              IDLE_ASSERT(
                  !me->promise_ &&
                  "An asynchronous wait on the group is already in progress!");

              bool initialized;
              if (me->initial_add_) {
                // Add present files initially
                me->add_files_initially();
                me->initial_add_ = false;
                initialized = true;

                IDLE_DETAIL_LOG_TRACE("Added initial files");
              } else {
                initialized = false;
              }

              if (!me->changes_.empty()) {
                if (initialized ||
                    has_elapsed(me->time_last_changed_, debounce_time)) {
                  // Resolve the previous promise in case any
                  // changes were made meanwhile
                  IDLE_DETAIL_LOG_TRACE("Resolving promise "
                                        "inplace with changes");
                  promise.set_value(std::move(me->changes_));
                  IDLE_ASSERT(me->changes_.empty());
                } else {
                  IDLE_DETAIL_LOG_TRACE("Storing promise for later "
                                        "resolution with debounce time");
                  // Resolve the promise later or when the timer has elapsed
                  me->promise_.emplace(PromisePair{std::move(promise), //
                                                   debounce_time});
                  me->debounce(clock_type::now() - me->time_last_changed_);
                }
              } else {
                IDLE_DETAIL_LOG_TRACE("Storing promise for later resolution "
                                      "because no change exists");

                // Store the promise for later resolution
                me->promise_.emplace(PromisePair{std::move(promise), //
                                                 debounce_time});
              }
            } else {
              IDLE_DETAIL_LOG_TRACE("Cancelling promise "
                                    "because actor went away");
              promise.set_canceled();
            }
          });
    }
  };
}

static auto action_name(efsw::Action action) noexcept {
  static constexpr auto enumeration = enumerate<efsw::Action>();
  return enum_name(enumeration, action);
}

void FileWatcherImpl::handleFileAction(efsw::WatchID /*watchid*/,
                                       std::string const& dir,
                                       std::string const& filename,
                                       efsw::Action action,
                                       std::string oldFilename) {
  using namespace boost::filesystem;

  std::string normalized = system_complete(path(dir) / filename)
                               .generic_string();

  IDLE_DETAIL_LOG_TRACE("Received dir: {}, filename: {} action: {}, "
                        "oldFilename: {}, normalized: {}",
                        dir, filename, action_name(action), oldFilename,
                        normalized);

  bool const is_filtered_out = !config_.filter(normalized);

  if (action != efsw::Action::Moved) {
    if (is_filtered_out) {
      IDLE_DETAIL_LOG_TRACE("Filtered out by predicate {}: {}",
                            action_name(action), normalized);
      return;
    }
  } else {
    auto const old = system_complete(path(dir) / oldFilename);
    oldFilename = old.generic_string();

    bool const old_is_filtered_out = !config_.filter(oldFilename);

    if (is_filtered_out) {
      if (old_is_filtered_out) {
        IDLE_DETAIL_LOG_TRACE("Filtered out by predicate {}: {}",
                              action_name(action), normalized);
        return;
      } else {
        normalized = std::exchange(oldFilename, {});
        action = efsw::Action::Delete;
      }
    } else {
      if (old_is_filtered_out) {
        action = efsw::Action::Add;
      }
    }
  }

  root().event_loop().dispatch(
      wrap(*this, [normalized = std::move(normalized),
                   old = std::move(oldFilename), action](auto&& me) mutable {
        IDLE_ASSERT(me->root().is_on_event_loop());

        switch (action) {
          case efsw::Action::Add: {
            IDLE_DETAIL_LOG_TRACE("Event Add: {}", normalized);

            /// If anything happened between mark the file as modified
            auto itr = me->changes_.find(normalized);
            if (itr != me->changes_.end()) {
              itr->second = file_event::FileModified{};
            } else {
              me->changes_[std::move(normalized)] = file_event::FileAdded{};
            }
            break;
          }
          case efsw::Action::Delete: {
            IDLE_DETAIL_LOG_TRACE("Event Delete: {}", normalized);
            me->changes_[std::move(normalized)] = file_event::FileRemoved{};
            break;
          }
          case efsw::Action::Modified: {
            IDLE_DETAIL_LOG_TRACE("Event Modified: {}", normalized);
            auto const itr = me->changes_.find(normalized);
            if (itr != me->changes_.end()) {
              if (get_if<file_event::FileAdded>(&itr->second)) {
                IDLE_DETAIL_LOG_TRACE("Merged a modify file event "
                                      "into file added event");
                break;
              }
            }
            me->changes_[std::move(normalized)] = file_event::FileModified{};
            break;
          }
          case efsw::Action::Moved: {
            IDLE_DETAIL_LOG_TRACE("Event Moved: {} (from {})", normalized, old);
            if (normalized != old) {
              me->changes_[std::move(normalized)] = file_event::FileRenamed{
                  std::move(old)};
            } else {
              me->changes_[std::move(normalized)] = file_event::FileModified{};
            }
            break;
          }
          default: {
            IDLE_DETAIL_UNREACHABLE();
            break;
          }
        }

        if (me->promise_) {
          me->debounce(me->promise_->debounce_time_);
        } else {
          me->time_last_changed_ = clock_type::now();
        }
      }));
}

continuable<> FileWatcherImpl::onStart() {
  return async([this] {
    IDLE_ASSERT(!promise_);

    IDLE_ASSERT(!debounce_timer_);
    debounce_timer_.emplace(instance_->ioContext());

    IDLE_ASSERT(watch_ids_.empty());
    watch_ids_.reserve(config_.watched.size());
    initial_add_ = config_.initial_add;

    for (Entry const& dir : config_.watched) {
      if (dir.initial_create && !boost::filesystem::exists(dir.path)) {
        boost::system::error_code ec;
        boost::filesystem::create_directories(dir.path, ec);

        if (ec) {
          IDLE_DETAIL_LOG_ERROR("Failed to create initial directory '{}' ({})!",
                                dir.path, ec.message());

          continue;
        }
      }

      efsw::WatchID id = instance_->addWatch(dir.path, this, dir.recursive);

      if (id >= 0) {
        watch_ids_.push_back(id);
      } else {
        IDLE_DETAIL_LOG_ERROR("Failed to add a watch to directory '{}' ('{}')!",
                              dir.path, efsw::Errors::Log::getLastErrorLog());
      }
    }
  });
}

continuable<> FileWatcherImpl::onStop() {
  return async([this] {
    removeWatches();

    debounce_timer_->cancel();
    debounce_timer_.reset();

    if (promise_) {
      if (!changes_.empty()) {
        promise_->promise_.set_value(std::move(changes_));
      } else {
        promise_->promise_.set_canceled();
      }

      promise_.reset();
      IDLE_ASSERT(!promise_);
    }

    changes_.clear();
  });
}

void FileWatcherImpl::removeWatches() {
  for (efsw::WatchID id : watch_ids_) {
    instance_->removeWatch(id);
  }
  watch_ids_.clear();
}

void FileWatcherImpl::debounce(duration debounce_time) {
  IDLE_ASSERT(root().is_on_event_loop());

  debounce_timer_->expires_from_now(debounce_time);

  debounce_timer_->async_wait(cti::use_continuable)
      .then(wrap(*this,
                 [](auto&& me) {
                   // The promise should be present here because we are not
                   // allowed to wait on file changes twice.
                   me->resolve();
                 }),
            root().event_loop().through_dispatch());
}

void FileWatcherImpl::resolve() {
  IDLE_ASSERT(root().is_on_event_loop());

  if (state().isRunning()) {
    IDLE_ASSERT(promise_);
    promise_->promise_.set_value(std::move(changes_));
    promise_.reset();
    IDLE_ASSERT(changes_.empty());
  }
}

bool FileWatcherImpl::has_elapsed(time_point time_point,
                                  duration duration) noexcept {
  return clock_type::now() - time_point >= duration;
}

void FileWatcherImpl::add_files_recursively(boost::filesystem::path const& dir,
                                            bool recursive) {

  using boost::make_iterator_range;
  using boost::filesystem::absolute;
  using boost::filesystem::directory_iterator;
  using boost::filesystem::is_directory;
  using boost::filesystem::path;

  for (auto&& file : make_iterator_range(directory_iterator(dir), {})) {
    bool const directory = is_directory(file);
    auto const file_path = file.path();
    auto file_str = file_path.generic_string();

    // Only mark the file as added if no event has occurred with it yet
    if (changes_.find(file_str) == changes_.end()) {
      if (config_.filter(file_str)) {
        changes_.insert(std::make_pair(std::move(file_str), //
                                       file_event::FileAdded{}));
      }
    }

    // Add files recursively if the option is present
    if (recursive && directory) {
      add_files_recursively(file_path, true);
    }
  }
}

void FileWatcherImpl::add_files_initially() {
  for (Entry const& dir : config_.watched) {
    if (boost::filesystem::is_directory(dir.path)) {
      add_files_recursively(dir.path, dir.recursive);
    } else {
      // TODO Fix file direct watches
      // changes_.insert(std::make_pair(dir.path, file_event::FileAdded{}));
    }
  }
}
} // namespace idle
