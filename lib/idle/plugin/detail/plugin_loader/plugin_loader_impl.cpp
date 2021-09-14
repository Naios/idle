
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

#include <boost/dll/shared_library.hpp>
#include <boost/filesystem/operations.hpp>
#include <idle/core/async.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/formatter.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/plugin/detail/plugin_loader/plugin_loader_impl.hpp>
#include <idle/plugin/plugin.hpp>

namespace idle {
struct SharedLibraryPredicate {
  bool operator()(StringView path) const noexcept {
    boost::filesystem::path const lib(path.begin(), path.end());
    if (!lib.has_extension()) {
      IDLE_DETAIL_LOG_TRACE("{} filtered out because no extension.",
                            lib.generic_string());
      return false; // Return if the file has no extension
    }

    // Allow the file to be watched if it has a shared library extension
    if (lib.extension() != boost::dll::shared_library::suffix()) {
      IDLE_DETAIL_LOG_TRACE(
          "{} filtered out because no shared library suffix ('{}').",
          lib.generic_string(),
          boost::dll::shared_library::suffix().generic_string());
      return false;
    }

    return true;
  }
};

void PluginLoaderImpl::setupImpl(Config config) {
  IDLE_ASSERT(state().isConfigurable());

  dirs_ = std::move(config.directories);
  IDLE_ASSERT(!dirs_.empty()); // TODO

  initial_load_ = config.initial_load;

  cache_directory_ = std::move(config.cache_directory);
  IDLE_ASSERT(!cache_directory_.empty()); // TODO
}

void PluginLoaderImpl::onSetup() {
  FileWatcher::Config config;
  config.watched = dirs_;
  config.initial_add = initial_load_;
  config.filter = SharedLibraryPredicate{};
  file_watcher_->setup(std::move(config));
}

continuable<> PluginLoaderImpl::onStart() {
  return async([this] {
    // TODO Add better error handling

    // Directories need to be set
    IDLE_ASSERT(!dirs_.empty());

    // Create the cache directory structure
    IDLE_ASSERT(!cache_directory_.empty());

    if (!exists(cache_directory_)) {
      create_directories(cache_directory_);
    }
  });
}

continuable<PluginLoader::PluginChanges> PluginLoaderImpl::watch_impl() {
  IDLE_ASSERT(state().isRunning()); // TODO convert this to lazy

  /*
  IDLE_ASSERT(root().is_on_event_loop());
   if (state().isRunning()) {
    return make_cancelling_continuable<PluginChanges>();
  }*/

  IDLE_DETAIL_LOG_DEBUG("Watching for plugin shared library changes...");

  return file_watcher_->watch().then(
      [weak = weakOf(this)](
          FileWatcher::FileChanges changes) -> continuable<PluginChanges> {
        IDLE_ASSERT(!changes.empty());

        IDLE_DETAIL_LOG_DEBUG(
            "Received {} changes related to shared libraries...",
            changes.size());

        if (auto me = weak.lock()) {
          auto plugin_changes = me->convert(std::move(changes));
          if (!plugin_changes.empty()) {
            return make_ready_continuable(std::move(plugin_changes));
          } else {
            return me->watch_impl();
          }
        } else {
          return make_cancelling_continuable<PluginChanges>();
        }
      },
      root().event_loop().through_post());
}

void PluginLoaderImpl::updateSideloadBannList(
    FileWatcher::FileChanges const& changes) {
  IDLE_ASSERT(root().is_on_event_loop());

  using namespace file_event;
  using namespace plugin_event;

  for (auto&& change : changes) {
    auto& path = change.first;
    auto const
        filename = boost::filesystem::path(path).filename().generic_string();

    visit(overload(
              [&](FileAdded const& /*fevent*/) mutable {
                plugin_source_->banSideload(filename);
              },
              [&](FileRemoved const& /*fevent*/) mutable {
                plugin_source_->unbanSideload(filename);
              },
              [&](FileModified const& /*fevent*/) mutable {
                //
              },
              [&](FileRenamed const& fevent) mutable {
                auto const old_filename = boost::filesystem::path(
                                              fevent.old_path)
                                              .filename()
                                              .generic_string();
                plugin_source_->unbanSideload(old_filename);
                plugin_source_->banSideload(filename);
              }),
          change.second);
  }
}

PluginLoader::PluginChanges
PluginLoaderImpl::convert(FileWatcher::FileChanges changes) {
  IDLE_ASSERT(root().is_on_event_loop());

  PluginChanges plugin_changes;

  using namespace file_event;
  using namespace plugin_event;

  IDLE_ASSERT(!changes.empty());

  ++generation_;

  updateSideloadBannList(changes);

  for (auto&& change : changes) {
    auto& path = change.first;
    visit(
        overload(
            [&](FileAdded /*fevent*/) mutable {
              IDLE_DETAIL_LOG_DEBUG("Plugin was added: {}", path);
              if (auto library = load(path)) {
                PluginAdded pevent{std::move(library)};
                plugins_.insert(std::make_pair(std::move(path), pevent.source));
                plugin_changes.push_back(std::move(pevent));
              }
            },
            [&](FileRemoved /*fevent*/) mutable {
              IDLE_DETAIL_LOG_DEBUG("Plugin was removed: {}", path);
              auto itr = plugins_.find(path);
              if (itr != plugins_.end()) {
                // Only notify the change if the plugin is still in use
                if (auto plugin = itr->second.lock()) {
                  plugin_changes.push_back(PluginRemoved{std::move(plugin)});
                }
                plugins_.erase(itr);
              }
            },
            [&](FileModified /*fevent*/) mutable {
              IDLE_DETAIL_LOG_DEBUG("Plugin was modified: {}", path);
              auto itr = plugins_.find(path);
              if (itr != plugins_.end()) {
                // Load a new version of the library and replace the old one
                if (auto plugin = itr->second.lock()) {
                  // Notify as modified if the old library is still in use
                  if (auto library = load(path)) {
                    itr->second = library;
                    plugin_changes.push_back(
                        PluginModified{std::move(plugin), std::move(library)});
                  }
                  return; // Exit the lambda
                }
              }

              // If not present or unused notify as added
              if (auto library = load(path)) {
                plugins_.insert(std::make_pair(std::move(path), library));
                plugin_changes.push_back(PluginAdded{std::move(library)});
              }
            },
            [&](FileRenamed fevent) mutable {
              IDLE_DETAIL_LOG_DEBUG("Plugin was renamed from \"{}\" to "
                                    "\"{}\"",
                                    fevent.old_path, path);
              auto itr = plugins_.find(fevent.old_path);
              if (itr != plugins_.end()) {
                // Load a new version of the library and replace the old one
                if (auto plugin = itr->second.lock()) {
                  // Notify as modified if the old library is still in use
                  if (auto library = load(path)) {
                    itr->second = library;
                    plugin_changes.push_back(
                        PluginModified{std::move(plugin), std::move(library)});
                  }
                  return; // Exit the lambda
                }
              }

              // If not present or unused notify as added
              if (auto library = load(path)) {
                plugins_.insert(std::make_pair(std::move(path), library));
                plugin_changes.push_back(PluginAdded{std::move(library)});
              }
            }),
        std::move(change.second));
  }

  return plugin_changes;
}

boost::filesystem::path
PluginLoaderImpl::unique_path_cache_of(boost::filesystem::path const& path) {
  IDLE_ASSERT(!cache_directory_.empty());

  auto const dir = cache_directory_ /
                   fmt::format(FMT_STRING("{}"), generation_);
  boost::filesystem::create_directory(dir);

  // Generate a new name for the file which includes the original name
  // and a unique counter which is incremented for every loading cycle when
  // a shared library is changed which is placed into the cache directory.
  return dir / path.filename();
}

Ref<Plugin> PluginLoaderImpl::load(std::string path) {
  auto cache_path = unique_path_cache_of(path).generic_string();
  auto plugin = plugin_source_->load(std::move(path), std::move(cache_path),
                                     generation_);
  return plugin;
}
} // namespace idle
