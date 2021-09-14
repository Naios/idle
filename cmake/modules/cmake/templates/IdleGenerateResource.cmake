#[[
#   _____    _ _        .      .    .
#  |_   _|  | | |  .       .           .
#    | |  __| | | ___         .    .        .
#    | | / _` | |/ _ \                .
#   _| || (_| | |  __/ github.com/Naios/idle
#  |_____\__,_|_|\___| AGPL v3 (Early Access)
#
# Copyright(c) 2018 - 2021 Denis Blank <denis.blank at outlook dot com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#]]

file(MD5 "${SOURCE_FILE}" SOURCE_CHECKSUM)

set(CACHED_CHECKSUM_FILE "${TARGET_FILE}.md5")
if(EXISTS "${CACHED_CHECKSUM_FILE}")
  file(READ "${CACHED_CHECKSUM_FILE}" CACHED_CHECKSUM)
endif()

if(SOURCE_CHECKSUM STREQUAL CACHED_CHECKSUM)
  return()
endif()

file(WRITE "${CACHED_CHECKSUM_FILE}" "${SOURCE_CHECKSUM}")

message(STATUS "Generating resource source from '${IDLE_RESOURCE_AS}'")

file(READ "${SOURCE_FILE}" SOURCE_CONTENT HEX)
string(LENGTH "${SOURCE_CONTENT}" SOURCE_CONTENT_LENGTH)

# Read blocks of 8 bytes to reduce the source file overhead
string(REGEX
       REPLACE "(..)(..)(..)(..)(..)(..)(..)(..)" "0x\\8\\7\\6\\5\\4\\3\\2\\1,"
               SOURCE_CONTENT "${SOURCE_CONTENT}")

# Replace the tail correctly (not divisible by the read block length)
string(REGEX REPLACE ",([0-9a-f]+)$" ",0x\\1" SOURCE_CONTENT
                     "${SOURCE_CONTENT}")

configure_file("${CMAKE_CURRENT_LIST_DIR}/idle_target_resource.in.cpp"
               "${TARGET_FILE}" @ONLY)
