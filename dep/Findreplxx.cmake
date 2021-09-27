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

include(IdleDependency)

idle_dependency(
  "AmokHuginnsson/replxx@0ff224b83145768785b4bf7c1d2b5ec0a9d28b30" #
  FILTER
  "src"
  "include"
  "examples" #
  RENAME
  "examples/util.h=include/util.h"
  "include=tmp"
  "tmp=include/replxx" #
  SUBDIRECTORY
  "${CMAKE_CURRENT_LIST_DIR}/replxx" #
  LICENSE_FILE
  "LICENSE.md" #
)
