
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

#include <boost/algorithm/string.hpp>
#include <idle/interface/arguments.hpp>

namespace idle {
std::string const& Arguments::get_string(std::size_t index) const {
  IDLE_ASSERT(index < args_.size());
  return args_[index];
}

std::size_t Arguments::get_uint(std::size_t index) const {
  IDLE_ASSERT(index < args_.size());
  return std::stoll(args_[index]);
}

Arguments Arguments::from(StringView str) {
  std::vector<std::string> args;
  boost::split(args, str, boost::is_any_of(" "), boost::token_compress_on);

  // Remove empty strings from the arguments
  args.erase(std::remove_if(args.begin(), args.end(),
                            [](std::string const& s) {
                              return s.empty();
                            }),
             args.end());

  return Arguments(std::move(args));
}
} // namespace idle
