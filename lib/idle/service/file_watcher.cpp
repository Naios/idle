
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

#include <idle/core/context.hpp>
#include <idle/core/ref.hpp>
#include <idle/service/detail/file_watcher/file_watcher_impl.hpp>
#include <idle/service/file_watcher.hpp>

namespace idle {
namespace file_event {
std::ostream& operator<<(std::ostream& os, FileAdded const& /*event*/) {
  return os << "FileAdded{}";
}

std::ostream& operator<<(std::ostream& os, FileRemoved const& /*event*/) {
  return os << "FileRemoved{}";
}

std::ostream& operator<<(std::ostream& os, FileModified const& /*event*/) {
  return os << "FileModified{}";
}

std::ostream& operator<<(std::ostream& os, FileRenamed const& event) {
  return os << "FileRenamed{\"from\": '" << event.old_path << "'}";
}
} // namespace file_event

continuable<FileWatcher::FileChanges> FileWatcher::watch() {
  return FileWatcherImpl::from(this)->watchImpl();
}

continuable<FileWatcher::FileChanges>
FileWatcher::watch(duration debounce_time) {
  return FileWatcherImpl::from(this)->watchImpl(debounce_time);
}

void FileWatcher::setup(Config config) {
  FileWatcherImpl::from(this)->setupImpl(std::move(config));
}

Ref<FileWatcher> FileWatcher::create(Inheritance parent) {
  return spawn<FileWatcherImpl>(std::move(parent));
}
} // namespace idle
