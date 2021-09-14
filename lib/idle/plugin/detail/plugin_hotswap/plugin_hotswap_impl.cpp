
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
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <idle/core/async.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/when_completed.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/views/filter.hpp>
#include <idle/core/views/transform.hpp>
#include <idle/interface/log.hpp>
#include <idle/plugin/detail/directories.hpp>
#include <idle/plugin/detail/plugin/plugin_impl.hpp>
#include <idle/plugin/detail/plugin_hotswap/plugin_hotswap_impl.hpp>
#include <mbedtls/sha1.h>

using boost::filesystem::path;

namespace idle {
static path root_location() {
  path location = boost::dll::program_location().parent_path();
  if (location.has_parent_path()) {
    return location.parent_path();
  } else {
    return location;
  }
}

inline auto hex(Span<unsigned char const> data) {
  static constexpr char const* hex_digits = "0123456789ABCDEF";
  return printable([data](std::ostream& os) {
    for (char value : data) {
      os.put(hex_digits[value & 0x0F]);
      os.put(hex_digits[(value >> 4) & 0x0F]);
    }
  });
}

static std::string abbrev_exe_loc_hash() {
  std::string const path = boost::dll::program_location().generic_string();

  unsigned char output[20];
  IDLE_CHECK(
      mbedtls_sha1_ret(reinterpret_cast<unsigned char const*>(path.data()),
                       path.size(), output) == 0);

  return fmt::format(FMT_STRING("{}"), hex({output, 4}));
}

static std::string default_cache_dir(Epoch epoch) noexcept {
  path const temp = boost::filesystem::temp_directory_path();
  path const executable = boost::dll::program_location();
  path const name = executable.stem();

  // TODO Calculate some id based on the pid and exe path
  // Currently we take the current epoch into account so that
  // the PDB debug information of MSVC is allowed to remain inside the cache.
  // We have to add a proper mechanic to unload PDBs that are automatically
  // loaded by MSVC.
  return absolute(temp / name / "cache" / abbrev_exe_loc_hash() / "hotswap" /
                  fmt::format(FMT_STRING("{}"), epoch))
      .generic_string();
}

void PluginHotswapImpl::onSetup() {
  if (config_.source_dir.empty()) {
    config_.source_dir = absolute(root_location() / "project").generic_string();
  } else {
    config_.source_dir = boost::filesystem::system_complete(config_.source_dir)
                             .generic_string();
  }

  if (config_.build_dir.empty()) {
    config_.build_dir = absolute(path(config_.source_dir) / "build")
                            .generic_string();
  } else {
    config_.build_dir = boost::filesystem::system_complete(config_.build_dir)
                            .generic_string();
  }

  if (config_.cache_dir.empty()) {
    config_.cache_dir = default_cache_dir(epoch());
  } else {
    config_.cache_dir = boost::filesystem::system_complete(config_.cache_dir)
                            .generic_string();
  }

  if (config_.module_dir.empty()) {
    config_.module_dir = absolute(root_location() / "share" / "cmake")
                             .generic_string();
  } else {
    config_.module_dir = boost::filesystem::system_complete(config_.module_dir)
                             .generic_string();
  }

  if (config_.watch.empty()) {
    config_.watch = {config_.source_dir};
  } else {
    for (auto& watch : config_.watch) {
      watch = boost::filesystem::system_complete(watch).generic_string();
    }
  }

  artifact_dir_ = absolute(path(config_.build_dir) / "artifacts");

  {
    // Set-up the plugin_loader
    PluginLoader::Config config;

    config.directories.emplace_back(artifact_dir_.generic_string(), true, true);

    config.initial_load = true;
    config.cache_directory = config_.cache_dir;
    loader_->setup(std::move(config));
  }

  {
    // Set-up the plugin_loader
    PluginRecompiler::Config config;
    config.watch = config_.watch;
    config.workspaces = config_.workspaces;
    config.build_dir = config_.build_dir;
    config.module_dir = config_.module_dir;
    config.source_dir = config_.source_dir;
    config.install_dir = artifact_dir_.generic_string();
    config.generator = config_.generator;

    recompiler_->setup(std::move(config));
  }
}

continuable<> PluginHotswapImpl::onStart() {
  return async([this] {
    return io_context_->async_post([this] {
      boost::filesystem::create_directories(config_.build_dir);
      boost::filesystem::create_directories(config_.module_dir);
      boost::filesystem::create_directories(config_.source_dir);

      /// Recreate the cache directory
      detail::reset_directory(config_.cache_dir);

      /// Recreate the artifact directory
      detail::reset_directory(artifact_dir_, config_.clean_artifacts);

      // For now just run the hotswap when the service is started
      run_impl();
    });
  });
}

void PluginHotswapImpl::setupImpl(Config config) {
  IDLE_ASSERT(state().isConfigurable());

  config_ = std::move(config);
}

continuable<> PluginHotswapImpl::run_impl() {
  return watch_sources_and_rebuild_for_first_successful_loop().then(
      when_all(watch_plugins_and_reload_loop(),
               watch_sources_and_rebuild_loop()));
}

continuable<> PluginHotswapImpl::dispatch_op_eager(plugin_added const& op) {
  IDLE_ASSERT(root().is_on_event_loop());

  IDLE_ASSERT(op.source);
  Plugin const& parent = *op.source;

  IDLE_ASSERT(bundles_.find(&parent) == bundles_.end());

  IDLE_DETAIL_LOG_DEBUG("Adding plugin: {}", parent);
  Ref<Bundle> instance = op.source->createBundle();
  Bundle& current = *instance;
  bundles_.insert(std::make_pair(&parent, std::move(instance)));

  return current.start();
}

continuable<> PluginHotswapImpl::dispatch_op_eager(plugin_removed const& op) {
  IDLE_ASSERT(root().is_on_event_loop());

  IDLE_ASSERT(op.source);
  Plugin& parent = *op.source;

  IDLE_DETAIL_LOG_DEBUG("Removing plugin: {}", parent);

  bundles_.erase(&parent);
  return parent.stop();
}

continuable<> PluginHotswapImpl::dispatch_op_eager(plugin_modified const& op) {
  IDLE_ASSERT(root().is_on_event_loop());
  IDLE_ASSERT(op.from->type() == op.to->type());
  IDLE_ASSERT(*op.from != *op.to);

  IDLE_DETAIL_LOG_DEBUG("Updating plugin: {} -> {}", *op.from, *op.to);

  Ref<Registry> reg = root().find(op.from->type());
  IDLE_ASSERT(reg);

  auto const interfaces = reg->interfaces();

  if (cast<Plugin>(interfaces.front()) != *op.to) {
    // The latest interface should always have the highest priority
    return make_ready_continuable();
  }

  bundles_.erase(op.from.get());
  Ref<Bundle> instance = op.to->createBundle();
  bundles_.insert(std::make_pair(op.to.get(), instance));

  if (auto other = interfaces.next()) {
    return when_completed(other | views::transform([](Interface& inter) {
                            IDLE_DETAIL_LOG_DEBUG("Stopping plugin {}",
                                                  inter.owner());
                            return inter.owner().stop();
                          }))
        .then(wrap(*this,
                   [updated = op.to.weak()](auto&& impl) -> continuable<> {
                     if (auto current = updated.lock()) {
                       auto const itr = impl->bundles_.find(current.get());
                       if (itr != impl->bundles_.end()) {
                         IDLE_DETAIL_LOG_DEBUG("Starting plugin bundle {}",
                                               *itr->second);
                         return itr->second->start();
                       }
                     }
                     return make_ready_continuable();
                   }),
              root().event_loop().through_post());
  } else {
    return instance->start();
  }
}

continuable<> PluginHotswapImpl::watch_plugins_and_reload_loop() {
  return loop(wrap(*this, [](auto&& me) {
    IDLE_DETAIL_LOG_DEBUG("Waiting for plugin changes in {}",
                          me->artifact_dir_.generic_string());

    return me->loader_->watch()
        .then(wrap(*me,
                   [](PluginLoader::PluginChanges changes,
                      auto&& me) -> continuable<> {
                     IDLE_ASSERT(me->root().is_on_event_loop());

                     if (changes.empty()) {
                       IDLE_DETAIL_LOG_TRACE(
                           "Skipping plugin changes, no changes receives!");
                       return make_ready_continuable();
                     } else if (changes.size() == 1U) {
                       IDLE_DETAIL_LOG_TRACE("Received 1 plugin change...");

                       return mpark::visit(
                           [&me](auto const& op) mutable {
                             return me->dispatch_op_eager(op);
                           },
                           changes.front());
                     } else {
                       // Always dispatch changes in the order: added ->
                       // removed such that depending services can swap to an
                       // alternative dependency when the current dependency
                       // is about to be removed.
                       IDLE_DETAIL_LOG_TRACE("Received {} plugin changes...",
                                             changes.size());

                       std::vector<continuable<>> ops;
                       ops.reserve(changes.size());

                       for (PluginLoader::PluginOperation const& change :
                            changes) {
                         if (auto const* op = get_if<plugin_added>(&change)) {
                           ops.emplace_back(me->dispatch_op_eager(*op));
                         }
                       }
                       for (PluginLoader::PluginOperation const& change :
                            changes) {
                         if (auto const* op = get_if<plugin_removed>(&change)) {
                           ops.emplace_back(me->dispatch_op_eager(*op));
                         }
                       }
                       for (PluginLoader::PluginOperation const& change :
                            changes) {
                         if (auto const* op = get_if<plugin_modified>(
                                 &change)) {
                           ops.emplace_back(me->dispatch_op_eager(*op));
                         }
                       }

                       return when_all(std::move(ops));
                     }
                   }),
              me->root().event_loop().through_post())
        .then(wrap(*me,
                   [](auto&& me) {
                     // Update the system
                     IDLE_DETAIL_LOG_DEBUG(
                         "Updating the system after plugin changes");

                     auto activity = me->activities_->add("Updating");

                     return me->root().update().next(
                         [activity = std::move(activity)](auto&&...) {
                           // Keep the activity valid
                           // Recover from am update cancellation
                           return make_result();
                         });
                   }),
              me->root().event_loop().through_post())
        .then([]() -> loop_result<> {
          IDLE_DETAIL_LOG_TRACE(
              "System update finished (after plugin changes)");
          return loop_continue();
        });
  }));
}

static void log_result(Log& log, result<> const& res) {
  IDLE_ASSERT(res.is_exception());

  if (res.get_exception()) {
    try {
      std::rethrow_exception(res.get_exception());
    } catch (std::exception const& e) {
      IDLE_LOG_ERROR(&log, "Your build has failed! ('{}')", e.what());
    } catch (...) {
      IDLE_LOG_ERROR(&log, "Your build has failed! ('unknown')");
    }
  } else {
    IDLE_LOG_ERROR(&log, "Your build was cancelled!");
  }
}

inline auto loop_cancel() {
  return make_plain(result<>::from(exception_arg_t{}, exception_t{}));
}

continuable<>
PluginHotswapImpl::watch_sources_and_rebuild_for_first_successful_loop() {
  return loop(wrap(*this, [](auto&& me) {
    IDLE_DETAIL_LOG_DEBUG("Waiting for first successful rebuild of {}",
                          me->config_.source_dir);

    return me->recompiler_->watchAndCompile(true).next(
        [weak = handleOf(*me)](auto&&... args) -> loop_result<> {
          auto const res = result<>::from(
              std::forward<decltype(args)>(args)...);

          if (res.is_value()) {
            IDLE_DETAIL_LOG_DEBUG("First rebuild was successful");

            return loop_break();
          } else {
            IDLE_ASSERT(res.is_exception());

            IDLE_DETAIL_LOG_DEBUG("First rebuild has failed, repeating...");

            if (auto me = weak.lock()) {
              log_result(*me->log_, res);

              if (res.get_exception()) {
                return loop_continue();
              } else {
                return loop_cancel();
              }
            } else {
              return loop_cancel();
            }
          }
        });
  }));
}

continuable<> PluginHotswapImpl::watch_sources_and_rebuild_loop() {
  return loop(wrap(*this, [](auto&& me) {
    IDLE_DETAIL_LOG_DEBUG("Waiting for source changes and rebuilds of {}",
                          me->config_.source_dir);

    return me->recompiler_->watchAndCompile().next(
        [weak = handleOf(*me)](auto&&... args) -> loop_result<> {
          auto const res = result<>::from(
              std::forward<decltype(args)>(args)...);

          if (auto me = weak.lock()) {
            if (res.is_value()) {
              return loop_continue();
            } else {
              IDLE_ASSERT(res.is_exception());

              log_result(*me->log_, res);

              if (res.get_exception()) {
                return loop_continue();
              } else {
                return loop_cancel();
              }
            }
          } else {
            return loop_cancel();
          }
        });
  }));
}
} // namespace idle
