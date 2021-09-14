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

add_library(idle-dep-base INTERFACE)
target_link_libraries(idle-dep-base INTERFACE idle-project-base)

target_compile_options(
  idle-dep-base
  INTERFACE $<$<CXX_COMPILER_ID:MSVC>: /W0 /wd4244 /wd4267>
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>: -w>)

target_compile_definitions(
  idle-dep-base INTERFACE $<$<CXX_COMPILER_ID:MSVC>: _SCL_SECURE_NO_WARNINGS
                          _CRT_SECURE_NO_WARNINGS>)
