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

set(IDLE_SQLITE_VERSION "3360000")

idle_dependency(
  "SQLite/SQLite3@3.36.0"
  URL
  "https://www.sqlite.org/2021/sqlite-amalgamation-${IDLE_SQLITE_VERSION}.zip"
  BASE_DIR
  "sqlite-amalgamation-${IDLE_SQLITE_VERSION}"
  SUBDIRECTORY
  "${CMAKE_CURRENT_LIST_DIR}/sqlite"
  RENAME
  "shell.c=src/shell.c"
  "sqlite3.c=src/sqlite3.c"
  "sqlite3ext.h=include/sqlite3/sqlite3ext.h"
  "sqlite3.h=include/sqlite3/sqlite3.h"
  NO_LICENSE_FILE)
