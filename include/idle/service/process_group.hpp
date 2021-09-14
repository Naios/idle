
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

#ifndef IDLE_SERVICE_PROCESS_GROUP_HPP_INCLUDED
#define IDLE_SERVICE_PROCESS_GROUP_HPP_INCLUDED

#include <exception>
#include <map>
#include <string>
#include <utility>
#include <idle/core/api.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/dep/function.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/dep/variant.hpp>
#include <idle/core/exception.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/service/sink.hpp>

namespace idle {
/// This exception is never thrown directly and it provides
/// a parent exception for all exceptions that are thrown
/// through a process spawn.
struct IDLE_API(idle) process_exception : std::exception {
  process_exception();
};

/// This exception is thrown directly if a process spawn fails to set up,
/// such as an invalid executable path.
struct IDLE_API(idle) process_spawn_exception : process_exception {
  process_spawn_exception();
};

/// This exception is thrown if the spawned child process exits
/// unexpectedly during runtime.
struct IDLE_API(idle) process_execution_exception : process_exception {
  process_execution_exception();
};

/// This exception is thrown if the spawned child process exits
/// with a non zero exit code and throw_on_bad_exit_code was set.
struct IDLE_API(idle) process_bad_exit_code_exception : process_exception {
  process_bad_exit_code_exception();

  /// Returns the exit code of the spawned process
  virtual int exit_code() const = 0;
};

/// A result of a process invocation, which provides its exit code.
class IDLE_API(idle) ProcessResultData : public ReferenceCounted {
public:
  ProcessResultData() = default;
  virtual ~ProcessResultData() = default;

  /// Returns the code the invoked process exited with.
  virtual int exit_code() const noexcept = 0;
};

/// A managed heap object which represents a process_result_data
using ProcessResult = Ref<ProcessResultData>;

/// A result of a process invocation, which provides the produced stdin
/// and stdout stream of the process.
class IDLE_API(idle) BufferedProcessResultData : public ProcessResultData {
public:
  /// Returns the completed stdout stream which was stored
  /// during process execution.
  StringView out() const noexcept;
  /// Returns the completed stderr stream which was stored
  /// during process execution.
  StringView err() const noexcept;
};

/// A managed heap object which represents a buffered_process_result
using BufferedProcessResult = Ref<BufferedProcessResultData>;

struct SpawnOptions {
  using Environment = std::map<std::string, std::string>;

  optional<std::string> working_dir;
  optional<Environment> env_var;
  bool inherit_env = true;
  bool throw_on_bad_exit_code = true;
};

struct StreamingOptions {
  Ref<Sink> out;
  Ref<Sink> err;
};

/// The process_group makes it possible to invoke child processes.
///
/// The group also makes it possible to terminate all spawned child
/// processes at once.
class IDLE_API(idle) ProcessGroup : public Service {
  friend struct internal_spawner;
  friend class ProcessGroupImpl;

public:
  using Service::Service;

  /// Creates a new process_group from the given context.
  static Ref<ProcessGroup> create(Inheritance parent);

  /// Spawns a child process asynchronously.
  ///
  /// \param executable A relative or absolute path to the spawned executable,
  ///        an executable which is located by the PATH variable must
  ///        be resolved through calling search_path first.
  ///        executable can be left empty, in that case the arguments are
  ///        forwarded to the system command processor.
  ///
  /// \param arguments The arguments that are passed to the process.
  ///
  /// \param env_var The environment variables the executable is executed with,
  ///                if the optional is left empty, the variables of the parent
  ///                process are used.
  ///
  /// \param inherit_env
  ///
  /// \param throw_on_bad_exit_code
  ///
  /// \returns A the continuable which resolves to a
  ///          buffered_process_result_view on success.
  ///          On failures following process_exception types
  ///          are thrown asynchronously:
  ///          - process_spawn_exception if the child process setup failed,
  ///            for instance if the executable wasn't found.
  ///          - process_execution_exception if the child process
  ///            terminated unexpected.
  ///          - process_bad_exit_code_exception if the spawned child process
  ///            exits with a non zero exit code and
  ///            throw_on_bad_exit_code was set.
  ///
  continuable<ProcessResult>
  spawnProcess(std::string executable, std::vector<std::string> arguments = {},
               SpawnOptions options = {});

  /// \copydoc spawnProcess
  continuable<BufferedProcessResult>
  spawnProcessBuffered(std::string executable,
                       std::vector<std::string> arguments = {},
                       SpawnOptions options = {});

  /// \copydoc spawnProcess
  continuable<ProcessResult>
  spawnProcessStreamed(std::string executable,
                       std::vector<std::string> arguments = {},
                       SpawnOptions options = {}, StreamingOptions stream = {});

  /// Opens a file with the operating system default editor
  continuable<> open(std::string file_path, SpawnOptions options = {});

  IDLE_SERVICE
}; // namespace idle

/// Returns the path to the native shell
IDLE_API(idle) std::string shellPath();

/// Searches the given executable name inside the path environment variable
///
/// \returns a present absolute path to the executable on success
IDLE_API(idle) optional<std::string> searchPath(StringView executable);

/// A literal which converts the given executable name into its absolut
/// path which is present inside the PATH variable.
///
/// \returns a path to the executable on success,
///          otherwise an empty string is returned.
IDLE_API(idle)
std::string operator"" _path(char const* executable, size_t length);
} // namespace idle

#endif // IDLE_SERVICE_PROCESS_GROUP_HPP_INCLUDED
