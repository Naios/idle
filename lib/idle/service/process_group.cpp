
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

#include <cstdio>
#include <istream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/strand.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/process.hpp>
#include <boost/process/extend.hpp>
#include <idle/core/context.hpp>
#include <idle/core/detail/exception_base.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/service/detail/process_group/process_group_impl.hpp>
#include <idle/service/process_group.hpp>

namespace idle {
process_exception::process_exception() = default;
process_spawn_exception::process_spawn_exception() = default;
process_execution_exception::process_execution_exception() = default;
process_bad_exit_code_exception::process_bad_exit_code_exception() = default;

StringView BufferedProcessResultData::out() const noexcept {
  return BufferedProcessResultDataImpl::from(this)->outImpl();
}

StringView BufferedProcessResultData::err() const noexcept {
  return BufferedProcessResultDataImpl::from(this)->errImpl();
}

Ref<ProcessGroup> ProcessGroup::create(Inheritance parent) {
  return spawn<ProcessGroupImpl>(std::move(parent));
}

continuable<ProcessResult>
ProcessGroup::spawnProcess(std::string executable,
                           std::vector<std::string> arguments,
                           SpawnOptions options) {
  return ProcessGroupImpl::from(this)->spawn_impl(std::move(executable),
                                                  std::move(arguments),
                                                  std::move(options));
}

continuable<BufferedProcessResult>
ProcessGroup::spawnProcessBuffered(std::string executable,
                                   std::vector<std::string> arguments,
                                   SpawnOptions options) {
  return ProcessGroupImpl::from(this)->spawn_buffered_impl(
      std::move(executable), std::move(arguments), std::move(options));
}

continuable<ProcessResult> ProcessGroup::spawnProcessStreamed(
    std::string executable, std::vector<std::string> arguments,
    SpawnOptions options, StreamingOptions stream) {
  return ProcessGroupImpl::from(this)->spawn_streamed_impl(
      std::move(executable), std::move(arguments), std::move(options),
      std::move(stream));
}

/// Opens a file with the operating system default editor
continuable<> ProcessGroup::open(std::string file_path, SpawnOptions options) {
  return ProcessGroupImpl::from(this)->open_impl(std::move(file_path),
                                                 std::move(options));
}

/*
continuable<process_result>
process_group::spawn_inplace(std::string executable,
                             std::vector<std::string> arguments,
                             spawn_options options) {
  return process_group_impl::of(this)->spawn_inplace_impl(
      std::move(executable), std::move(arguments), std::move(options));
}

continuable<buffered_process_result>
process_group::spawn_buffered(std::string executable,
                              std::vector<std::string> arguments,
                              spawn_options options) {
  return process_group_impl::of(this)->spawn_buffered_impl(
      std::move(executable), std::move(arguments), std::move(options));
}

continuable<process_result>
process_group::spawn_streamed(std::string executable,
                              std::vector<std::string> arguments,
                              spawn_options options, process_sink_t sink) {
  return process_group_impl::of(this)->spawn_streamed_impl(
      std::move(executable), std::move(arguments), std::move(options),
      std::move(sink));
}*/

std::string shellPath() {
  return boost::process::shell().string();
}

optional<std::string> searchPath(StringView executable) {
  using boost::filesystem::path;

  path const converted(executable.begin(), executable.end());
  path const found = boost::process::search_path(converted);
  if (found.empty()) {
    return {};
  } else {
    return found.generic_string();
  }
}

std::string operator"" _path(char const* executable, size_t length) {
  if (auto path = searchPath(StringView(executable, length))) {
    return std::move(*path);
  } else {
    return {};
  }
}
} // namespace idle
