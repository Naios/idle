
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

#include <fstream>
#include <regex>
#include <streambuf>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <fmt/color.h>
#include <fmt/format.h>
#include <idle/core/async.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/format_ext.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/platform.hpp>
#include <idle/core/use.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/version.hpp>
#include <idle/interface/log.hpp>
#include <idle/plugin/detail/plugin_compiler/plugin_compiler_impl.hpp>
#include <idle/service/process_group.hpp>
#include <nlohmann/json.hpp>

namespace idle {
static void cmake_escape(std::string& str) {
#ifdef IDLE_PLATFORM_WINDOWS
  boost::algorithm::replace_all(str, " ", "\\ ");
#endif
}

static bool is_vs_generator(std::string const& generator) noexcept {
#ifdef IDLE_PLATFORM_WINDOWS
  return generator.find("Visual Studio") != std::string::npos;
#else
  (void)generator;
  return false;
#endif
}

/// Returns true when the generator uses a build Config instead
/// of a statically defined CMAKE_BUILD_TYPE.
static bool is_config_generator(std::string const& generator) noexcept {
  return is_vs_generator(generator) || generator == "Ninja Multi-Config";
}

static bool is_ninja_generator(std::string const& generator) noexcept {
  return generator.find("Ninja") != std::string::npos;
}

#ifdef IDLE_PLATFORM_WINDOWS
static std::string const vcvarsall_bat_path =
    "C:\\Program Files (x86)\\Microsoft Visual "
    "Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat";
static std::string const arch = "x64";

static SpawnOptions::Environment assignmentsToEnv(StringView output) noexcept {
  SpawnOptions::Environment env;
  static std::regex const reg("([^=]+)=([^\n\r]+)\r?\n");

  using Iter = std::regex_iterator<StringView::iterator>;

  Iter begin(output.begin(), output.end(), reg);
  Iter end;

  for (Iter i = begin; i != end; ++i) {
    auto const& match = *i;

    if (match.size() != 3) {
      IDLE_ASSERT(!match.empty());

      IDLE_DETAIL_LOG_ERROR("Failed to split an env var (not displayed)",
                            match.size());
      continue;
    }

    // 0 is the whole string, 1 the key and 2 the value
    env.insert(std::make_pair(match.str(1), match.str(2)));
  }

  return env;
}

static void removeFromEnv(SpawnOptions::Environment& env,
                          SpawnOptions::Environment const& to_remove) noexcept {

  for (auto&& rem : to_remove) {
    auto const itr = env.find(rem.first);
    if (itr == env.end()) {
      continue;
    }

    if (itr->second != rem.second) {
      // The value of the env variable has changed, preserve it
      continue;
    }

    env.erase(rem.first);
  }
}

/// Splits an env var assignment list from the Windows 'set' command into
/// key value pairs.
static auto getVSEnvVars(Activities& activities, ProcessGroup& process) {
  auto activity = activities.add("Caching VC Vars");

  return (process.spawnProcessBuffered({}, {"call", vcvarsall_bat_path, arch,
                                            ">", "NUL", "&&", "set"}) &&
          process.spawnProcessBuffered({}, {"set"}))
      .then([activity = std::move(activity)](BufferedProcessResult vcvars,
                                             BufferedProcessResult baseline)
                -> SpawnOptions::Environment {
        // We calculate the diff between a normal 'set' output and the one
        // containing the 'vcvarsall.bat' env variables.
        //
        // Through that we try not to store your env variables on disk,
        // because this could make it easy to leak sensitive information.
        //
        // Variables with changed values also are cached (in addition to new
        // ones set by 'vcvarsall.bat').
        auto env_vc = assignmentsToEnv(vcvars->out());
        auto const env_base = assignmentsToEnv(baseline->out());
        removeFromEnv(env_vc, env_base);

        return env_vc;
      });
}
#endif

void PluginCompilerImpl::setupImpl(Config config) {
  IDLE_ASSERT(state().isConfigurable());
  IDLE_ASSERT(root().is_on_event_loop());

  config_ = std::move(config);
  IDLE_ASSERT(!config_.build_dir.empty());   // TODO
  IDLE_ASSERT(!config_.install_dir.empty()); // TODO
  IDLE_ASSERT(!config_.source_dir.empty());  // TODO

  // Retrieve the CMake exe from the path
  if (config_.cmake_exe.empty()) {
    config_.cmake_exe = "cmake"_path;
  }
  // Otherwise use the built in CMake command
  if (config_.cmake_exe.empty()) {
    StringView const cmd = version::cmake_command;
    config_.cmake_exe.insert(config_.cmake_exe.begin(), cmd.begin(), cmd.end());
  }

  // Insert the CMake generator from the built in one
  if (config_.generator.empty()) {
    StringView const gen = version::cmake_generator;

    // Use ninja when available
    std::string const ninja = "ninja"_path;
    if (!ninja.empty()) {
      // CMake 3.17 doesnt support pch for Ninja Multi-Config yet properly
      // #ifdef IDLE_PLATFORM_WINDOWS
      //   config_.generator = "Ninja Multi-Config";
      // #else
      config_.generator = "Ninja";
      // #endif
    } else {
      config_.generator.insert(config_.generator.begin(), gen.begin(),
                               gen.end());
    }
  }

  // Insert the CMake build type from the built in one
  if (config_.build_type.empty()) {
    StringView const type = version::cmake_build_type;
    config_.build_type.insert(config_.build_type.begin(), type.begin(),
                              type.end());
  }

  is_config_generator_ = is_config_generator(config_.generator);

  // Escape the paths passed to CMake through -D
  cmake_escape(config_.install_dir);
  cmake_escape(config_.module_dir);
}

/// Adds default environment variables for all platforms that are added
/// to the compiler and build system invocation.
static void addDefaultEnvVars(SpawnOptions::Environment& env) noexcept {
  // Forces color output of make tools targeting especially Ninja on Windows
  env["CLICOLOR_FORCE"] = "1";
}

continuable<> PluginCompilerImpl::onStart() {
  return async([this]() -> continuable<> {
    compiler_env_ = {};

#ifdef IDLE_PLATFORM_WINDOWS
    if (is_ninja_generator(config_.generator)) {
      if (config_.cache_build_env) {
        if (auto env = readCompilerEnvCache()) {
          compiler_env_ = std::move(*env);
        } else {
          return getVSEnvVars(*activities_, *process_group_)
              .then([this](SpawnOptions::Environment env) {
                if (config_.cache_build_env) {
                  writeCompilerEnvCache(env);
                }

                compiler_env_ = std::move(env);
                addDefaultEnvVars(compiler_env_);
              });
        }
      }
    }
#endif // IDLE_PLATFORM_WINDOWS

    addDefaultEnvVars(compiler_env_);
    return make_ready_continuable();
  });
}

continuable<> PluginCompilerImpl::generate_impl() {
  return async_on(wrap(*this,
                       [](auto&& me) mutable -> continuable<> {
                         boost::filesystem::path cache = me->config_.build_dir;
                         cache /= "CMakeCache.txt";

                         if (boost::filesystem::exists(cache)) {
                           return me->cmake_regenerate();
                         } else {
                           return me->cmake_configure();
                         }
                       }),
                  root().event_loop().through_post());
}

continuable<> PluginCompilerImpl::build_impl(std::string target) {
  return async_on(
      wrap(*this,
           [target = std::move(target)](auto&& me) mutable {
             IDLE_ASSERT(!me->config_.cmake_exe.empty());

             IDLE_LOG_INFO(me->log_, "Building target {}", target);

             IDLE_DETAIL_LOG_DEBUG(
                 "Building target {} from cmake build dir '{}'...", target,
                 me->config_.build_dir);

             auto activity = me->activities_->add(
                 fmt::format(FMT_STRING("Building {}"), //
                             target));

             std::vector<std::string> args{"--build", me->config_.build_dir,
                                           "--target", std::move(target)};

             if (me->is_config_generator_ && !me->config_.build_type.empty()) {
               args.insert(args.end(), {"--config", me->config_.build_type});
             }

             SpawnOptions options;
             options.working_dir = me->config_.build_dir;
             options.env_var = me->compiler_env_;
             IDLE_ASSERT(options.inherit_env);

             return me->process_group_
                 ->spawnProcessStreamed(me->config_.cmake_exe, std::move(args),
                                        options, me->stream())
                 .then([activity = std::move(activity)] {
                   // Keep the activity valid
                 });
           }),
      root().event_loop().through_post());
}

continuable<> PluginCompilerImpl::build_install_impl() {
  return async(weakly(handleOf(*this), [](auto&& me) {
#ifdef IDLE_PLATFORM_WINDOWS
    if (is_vs_generator(me->config_.generator)) {
      return me->build_impl("INSTALL");
    }
#endif
    return me->build_impl("install");
  }));
}

continuable<>
PluginCompilerImpl::install_component_impl(std::string component) {
  return async_on(
      wrap(*this,
           [component = std::move(component)](auto&& me) mutable {
             IDLE_ASSERT(!me->config_.cmake_exe.empty());

             IDLE_LOG_INFO(me->log_, "Installing component {}", component);

             IDLE_DETAIL_LOG_DEBUG(
                 "Installing component {} from cmake build dir '{}'...",
                 component, me->config_.build_dir);

             std::vector<std::string> args{
                 format(FMT_STRING("-DCOMPONENT={}"), component)};

             if (me->is_config_generator_ && !me->config_.build_type.empty()) {
               args.emplace_back(format(FMT_STRING("-DBUILD_TYPE={}"),
                                        me->config_.build_type));
             }

             args.insert(args.end(),
                         {"-P",
                          boost::filesystem::absolute("cmake_install.cmake",
                                                      me->config_.build_dir)
                              .generic_string()});

             // Process spawn options
             SpawnOptions options;
             options.working_dir = me->config_.build_dir;
             options.env_var = me->compiler_env_;
             IDLE_ASSERT(options.inherit_env);

             auto activity = me->activities_->add(
                 fmt::format(FMT_STRING("Installing {}"), //
                             component));

             return me->process_group_
                 ->spawnProcessStreamed(me->config_.cmake_exe, std::move(args),
                                        std::move(options), me->stream())
                 .then([activity = std::move(activity)] {
                   // Keep the activity valid
                 });
           }),
      root().event_loop().through_post());
}

void PluginCompilerImpl::log(LogLevel level,
                             LogMessage const& message) noexcept {

  StringView const data = message.message;

  if (data.find("error") != StringView::npos) {
    // Some build systems like Ninja ignore stderr and pipe everything to stdout
    LogLevel const severity = std::max(level, LogLevel::error);

    if (log_->is_enabled(severity)) {
      fmt::memory_buffer buffer;
      format_to(buffer,
                fmt::bg(fmt::terminal_color::bright_red) |
                    fmt::fg(fmt::terminal_color::bright_white),
                "{}", data);

      log_->log(severity, LogMessage({buffer.data(), buffer.size()}));
    }

  } else if (data.find("warn") != StringView::npos) {
    // Warnings usually go into stdout thus we need to forward it to the warning
    // log level.
    LogLevel const severity = std::max(level, LogLevel::warn);

    if (log_->is_enabled(severity)) {
      fmt::memory_buffer buffer;
      format_to(buffer,
                fmt::bg(fmt::terminal_color::yellow) |
                    fmt::fg(fmt::terminal_color::bright_white),
                "{}", data);

      log_->log(severity, LogMessage({buffer.data(), buffer.size()}));
    }

  } else {
    if (log_->is_enabled(level)) {
      log_->log(level, LogMessage(data));
    }
  }
}

continuable<> PluginCompilerImpl::cmake_configure() {
  IDLE_ASSERT(!config_.cmake_exe.empty());

  IDLE_LOG_INFO(log_, "Configuring build directory '{}'", config_.build_dir);

  IDLE_DETAIL_LOG_DEBUG("Configuring cmake build dir '{}'...",
                        config_.build_dir);

  // CMake arguments
  std::vector<std::string> args{config_.source_dir,
                                format(FMT_STRING("-DCMAKE_INSTALL_PREFIX={}"),
                                       config_.install_dir),
                                "-DBUILD_SHARED_LIBS=ON",
                                "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"};

  if (!config_.module_dir.empty()) {
    args.emplace_back(
        format(FMT_STRING("-DCMAKE_MODULE_PATH={}"), config_.module_dir));
  }

  if (!is_config_generator_ && !config_.build_type.empty()) {
    args.emplace_back(
        format(FMT_STRING("-DCMAKE_BUILD_TYPE={}"), config_.build_type));
  }

  if (!config_.generator.empty()) {
    args.insert(args.end(), {"-G", config_.generator});
  }

  // Process spawn options
  SpawnOptions options;
  options.working_dir = config_.build_dir;
  options.env_var = compiler_env_;
  IDLE_ASSERT(options.inherit_env);

  auto activity = activities_->add("Configuring");

  return process_group_
      ->spawnProcessStreamed(config_.cmake_exe, std::move(args),
                             std::move(options), stream())
      .then([activity = std::move(activity)] {
        // Keep the activity valid
      });
}

continuable<> PluginCompilerImpl::cmake_regenerate() {
  IDLE_LOG_INFO(log_, "Regenerating build directory '{}'", config_.build_dir);

  IDLE_DETAIL_LOG_DEBUG("Regenerating cmake build dir '{}'...",
                        config_.build_dir);

  SpawnOptions options;
  options.working_dir = config_.build_dir;
  options.env_var = compiler_env_;
  IDLE_ASSERT(options.inherit_env);

  auto activity = activities_->add("Regenerating");

  return process_group_
      ->spawnProcessStreamed(config_.cmake_exe, {"."}, std::move(options),
                             stream())
      .then([activity = std::move(activity)] {
        // Keep the activity valid
      });
}

#ifdef IDLE_PLATFORM_WINDOWS
static auto compilerEnvCachePath(std::string const& build_dir) noexcept {
  boost::filesystem::path cache = build_dir;
  cache /= fmt::format(FMT_STRING("IdleCompilerEnvCache_{}.json"), arch);
  return cache;
}

bool PluginCompilerImpl::writeCompilerEnvCache(
    SpawnOptions::Environment const& env) const {
  boost::filesystem::path const cache = compilerEnvCachePath(config_.build_dir);

  IDLE_DETAIL_LOG_TRACE("Writing compiler env cache to '{}'....",
                        cache.generic_string());

  boost::filesystem::path const cache_dir = cache.parent_path();

  boost::system::error_code ec;
  boost::filesystem::create_directories(cache_dir, ec);
  if (ec) {
    IDLE_DETAIL_LOG_ERROR("Failed to create the build dir '{}' ('{}')!",
                          cache_dir.generic_string(), ec.message());
    return false;
  }

  std::ofstream file(cache.c_str(), std::ofstream::trunc);
  if (!file.is_open()) {
    IDLE_DETAIL_LOG_ERROR("Failed to write compiler env cache {}!",
                          cache.generic_string());
    return false;
  }

  std::chrono::steady_clock::time_point const
      expiry = (std::chrono::steady_clock::now() + config_.cache_expiry);

  nlohmann::json data{{"expires", expiry.time_since_epoch().count()},
                      {"env", std::move(env)}};

  file << data.dump(2);
  return true;
}

optional<SpawnOptions::Environment>
PluginCompilerImpl::readCompilerEnvCache() const noexcept {
  boost::filesystem::path const cache = compilerEnvCachePath(config_.build_dir);

  IDLE_DETAIL_LOG_TRACE("Reading compiler env cache from '{}'....",
                        cache.generic_string());

  std::ifstream file(cache.c_str());
  if (!file.is_open()) {
    return {};
  }

  try {
    nlohmann::json data = nlohmann::json::parse(
        std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    using rep = std::chrono::steady_clock::time_point::rep;
    auto const expiry = data["expires"].get<rep>();

    if (expiry <= std::chrono::steady_clock::now().time_since_epoch().count()) {
      IDLE_DETAIL_LOG_DEBUG("Cache '{}' has expired.", cache.generic_string());
      return {}; // The cache has expired
    }

    return data["env"].get<SpawnOptions::Environment>();
  } catch (std::exception const& e) {
    IDLE_DETAIL_LOG_ERROR("Failed to parse {} ({}).", cache.generic_string(),
                          e.what());
    (void)e;
    return {};
  }
}
#endif

StreamingOptions PluginCompilerImpl::stream() {
  StreamingOptions options;

  if (config_.colorize) {
    options.out = refOf(this).pin(sink(LogLevel::debug));
    options.err = refOf(this).pin(sink(LogLevel::error));
  } else {
    options.out = refOf(this).pin(log_->sink(LogLevel::debug));
    options.err = refOf(this).pin(log_->sink(LogLevel::error));
  }

  return options;
}
} // namespace idle
