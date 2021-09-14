
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

#ifndef IDLE_DETAIL_FOR_EACH_MAPPED_TYPE
#  define IDLE_DETAIL_FOR_EACH_MAPPED_TYPE(NAME)
#endif // IDLE_DETAIL_FOR_EACH_MAPPED_TYPE

#ifndef IDLE_DETAIL_FOR_EACH_MAPPED_STRUCTURE
#  define IDLE_DETAIL_FOR_EACH_MAPPED_STRUCTURE(NAME)                          \
    IDLE_DETAIL_FOR_EACH_MAPPED_TYPE(NAME)
#endif // IDLE_DETAIL_FOR_EACH_MAPPED_STRUCTURE

#ifndef IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE
#  define IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(NAME, TYPE)                    \
    IDLE_DETAIL_FOR_EACH_MAPPED_TYPE(NAME)
#endif // IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE

IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(Bool, bool)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(Int8, std::int8_t)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(Int16, std::int16_t)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(Int32, std::int32_t)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(Int64, std::int64_t)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(UInt8, std::uint8_t)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(UInt16, std::uint16_t)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(UInt32, std::uint32_t)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(UInt64, std::uint64_t)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(Float, float)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(Double, double)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(String, std::string)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(TimePoint,
                                      std::chrono::system_clock::time_point)
IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE(Duration,
                                      std::chrono::system_clock::duration)

IDLE_DETAIL_FOR_EACH_MAPPED_STRUCTURE(Array)
IDLE_DETAIL_FOR_EACH_MAPPED_STRUCTURE(Set)
IDLE_DETAIL_FOR_EACH_MAPPED_STRUCTURE(Object)

IDLE_DETAIL_FOR_EACH_MAPPED_TYPE(Empty)

#undef IDLE_DETAIL_FOR_EACH_MAPPED_PRIMITIVE
#undef IDLE_DETAIL_FOR_EACH_MAPPED_STRUCTURE
#undef IDLE_DETAIL_FOR_EACH_MAPPED_TYPE
