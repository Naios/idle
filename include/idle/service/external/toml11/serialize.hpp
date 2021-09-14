
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

#ifndef IDLE_SERVICE_EXTERNAL_TOML11_SERIALIZE_HPP_INCLUDED
#define IDLE_SERVICE_EXTERNAL_TOML11_SERIALIZE_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/util/nullable.hpp>
#include <idle/service/art/reflection.hpp>
#include <idle/service/sink.hpp>
#include <toml11/toml.hpp>

namespace idle {
IDLE_API(idle)
void toml_serialize(toml::basic_value<toml::preserve_comments>& toml,
                    ConstReflectionPtr ptr);

IDLE_API(idle)
bool toml_deserialize(toml::basic_value<toml::preserve_comments> const& toml,
                      ReflectionPtr ptr, Nullable<Sink> out = {});

} // namespace idle

#endif // IDLE_SERVICE_EXTERNAL_TOML11_SERIALIZE_HPP_INCLUDED
