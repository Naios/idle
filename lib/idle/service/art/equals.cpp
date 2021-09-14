
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

#include <idle/service/art/equals.hpp>
#include <idle/service/external/toml11/serialize.hpp>
#include <toml11/toml.hpp>

namespace idle {
namespace art {
bool equals(ConstReflectionPtr left, ConstReflectionPtr right) noexcept {
  // TODO Improve this without any memory allocation
  // (this is a quick solution for now)

  toml::basic_value<toml::preserve_comments> left_toml;
  toml::basic_value<toml::preserve_comments> right_toml;

  toml_serialize(left_toml, left);
  toml_serialize(right_toml, right);

  return left_toml == right_toml;
}
} // namespace art
} // namespace idle
