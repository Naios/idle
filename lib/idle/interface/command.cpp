
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

#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <idle/interface/command.hpp>

namespace idle {
Session::~Session() {}

Command::Command(Service& owner)
  : Interface(owner) {
  // Force the vtable into the DLL
}

void Command::onPartInit() noexcept {
  // Initialize the command name
  current_command_name_ = command_name();

  Interface::onPartInit();
}

std::string Command::command_name() const noexcept {
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, FMT_STRING("{}"), owner().name());

  std::string str(buffer.begin(), buffer.end());

  // Correct names provided by RTTI
  boost::replace_all(str, "::", " ");

  if (!str.empty()) {
    return str;
  } else {
    // Gracefully generate a unique command name when there is no name given
    return format(FMT_STRING("unknown cmd_{}_{}"), owner().guid(),
                  partOffset());
  }
}

StringView Command::current_command_name() const noexcept {
  IDLE_ASSERT(owner().state().isInitializedUnsafe());
  return current_command_name_;
}

bool Command::operator>(Interface const& other) const noexcept {
  auto const& left = current_command_name_;
  auto const& right = cast<Command>(other).current_command_name_;

  return std::lexicographical_compare(left.begin(), left.end(), right.begin(),
                                      right.end());
}
} // namespace idle
