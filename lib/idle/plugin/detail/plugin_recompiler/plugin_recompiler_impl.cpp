
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

#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <fmt/ostream.h>
#include <idle/core/async.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/dep/variant.hpp>
#include <idle/core/detail/formatter.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/use.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/plugin/detail/plugin_recompiler/plugin_recompiler_impl.hpp>

namespace idle {
static bool is_cxx_file(StringView file) {
  static std::regex const regex(".*\\.(h|hh|hpp|c|cc|cpp)$");
  return std::regex_match(file.begin(), file.end(), regex);
}

static bool is_cxx_source_file(StringView file) {
  static std::regex const regex(".*\\.(c|cc|cpp)$");
  return std::regex_match(file.begin(), file.end(), regex);
}

static bool is_cmake_file(boost::filesystem::path const& file) {
  if (file.filename() == "CMakeLists.txt") {
    return true;
  }

  return file.extension().generic_string() == ".cmake";
}

struct IsReloadableSourceFilePredicate {
  bool operator()(StringView path) const noexcept {
    boost::filesystem::path const file{path.begin(), path.end()};
    auto const extension = file.extension().generic_string();
    return extension.empty() || is_cxx_file(extension) || is_cmake_file(file);
  }
};

void PluginRecompilerImpl::setupImpl(Config config) {
  IDLE_ASSERT(state().isConfigurable());
  config_ = std::move(config);
}

void PluginRecompilerImpl::onSetup() {
  plugin_compiler_->setup(config_);

  {
    FileWatcher::Config config;
    config.watched.reserve(config_.watch.size());
    for (std::string const& dir : config_.watch) {
      config.watched.emplace_back(dir, true, false);
    }
    config.initial_add = true;
    config.filter = IsReloadableSourceFilePredicate{};

    file_watcher_->setup(std::move(config));
  }

  for (std::string& workspace : config_.workspaces) {
    auto const path = boost::filesystem::system_complete(workspace);
    workspace = path.generic_string();
  }

  failed_last_ = false;
}

static bool needs_regenerate(FileWatcher::FileChanges const& changes) noexcept {

  for (auto const& change : changes) {

    FileWatcher::FileOperation const& op = change.second;

#if IDLE_DETAIL_HAS_LOG_LEVEL <= IDLE_DETAIL_LOG_LEVEL_TRACE
    mpark::visit(
        [](auto const& ev) {
          IDLE_DETAIL_LOG_TRACE("needs_regenerate receives: {}", ev);
        },
        op);
#endif

    if (get_if<file_event::FileModified>(&op) == nullptr) {
      return true;
    }

    boost::filesystem::path const p = change.first;

    if (!p.has_extension()) {
      return true;
    }

    if (is_cmake_file(p)) {
      return true;
    }

    if (!is_cxx_file(p.extension().generic_string())) {
      return true;
    }
  }

  return false;
}

bool isFileInWorkSpace(StringView workspace, StringView file) noexcept {
  IDLE_ASSERT(workspace);
  IDLE_ASSERT(file);

  if (workspace.size() > file.size()) {
    return false;
  } else {
    return workspace == file.substr(0, workspace.size());
  }
}

std::string getComponentDir(std::vector<std::string> const& workspaces,
                            std::string const& file) {

  for (std::string const& workspace : workspaces) {
    if (!isFileInWorkSpace(workspace, file)) {
      continue;
    }

    boost::filesystem::path rel = boost::filesystem::relative(file, workspace);
    while (rel.has_parent_path()) {
      rel = rel.parent_path();
    }

    if (rel.has_extension()) {
      rel.replace_extension({});
    }

    return rel.generic_string();
  }

  return {};
}

static constexpr std::size_t FILE_CHANGED_THRESHOLD = 5;

static std::string
getInstallComponent(std::vector<std::string> const& workspaces,
                    FileWatcher::FileChanges const& changes) noexcept {

  if (workspaces.empty() || (changes.size() > FILE_CHANGED_THRESHOLD)) {
    return {};
  }

  std::string component;
  for (auto const& change : changes) {
    // If there is a different op than modified initiate a full rebuild
    if (get_if<file_event::FileModified>(&change.second) == nullptr) {
      return {};
    }

    // If there was header or different file modified initiate a full rebuild
    if (!is_cxx_source_file(change.first)) {
      return {};
    }

    std::string current = getComponentDir(workspaces, change.first);
    if (current.empty()) {
      return {};
    } else if (component.empty()) {
      component = std::move(current);
    } else if (component != current) {
      return {};
    }
  }

  return component;
}

continuable<>
PluginRecompilerImpl::watch_and_compile_impl(bool force_regenerate) {
  IDLE_ASSERT(state().isRunning());

  return async_on(wrap(*this,
                       [](auto&& me) mutable {
                         IDLE_DETAIL_LOG_DEBUG(
                             "Watching dir \"{}\" for possible rebuilds",
                             me->config_.source_dir);
                         return me->file_watcher_->watch();
                       }),
                  root().event_loop().through_dispatch())
      .then(wrap(*this,
                 [force_regenerate](FileWatcher::FileChanges changes,
                                    auto&& me) mutable -> continuable<> {

#if IDLE_DETAIL_HAS_LOG_LEVEL <= IDLE_DETAIL_LOG_LEVEL_TRACE
                   for (auto const& change : changes) {
                     IDLE_DETAIL_LOG_TRACE("Received source file "
                                           "change on '{}'",
                                           change.first);
                   }
#endif

                   bool const regenerate = needs_regenerate(changes);
                   if (force_regenerate || regenerate) {
                     return me->plugin_compiler_->generate().then(
                         me->plugin_compiler_->buildInstall());
                   } else {
                     std::string const component = getInstallComponent(
                         me->config_.workspaces, changes);

                     if (component.empty()) {
                       return me->plugin_compiler_->buildInstall();
                     } else {
                       return me->plugin_compiler_->build(component).then(
                           me->plugin_compiler_->installComponent(component));
                     }
                   }
                 }),
            root().event_loop().through_dispatch());
}

} // namespace idle
