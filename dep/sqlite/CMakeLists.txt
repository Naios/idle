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

add_library(sqlite STATIC ${SQLite3_DIR}/src/sqlite3.c
                          ${SQLite3_DIR}/include/sqlite3/sqlite3.h)

target_include_directories(sqlite PUBLIC ${SQLite3_DIR}/include)

target_link_libraries(sqlite PRIVATE idle-dep-base)

target_compile_features(
  sqlite
  PUBLIC cxx_alias_templates
         cxx_auto_type
         cxx_constexpr
         cxx_decltype
         cxx_decltype_auto
         cxx_final
         cxx_lambdas
         cxx_generic_lambdas
         cxx_variadic_templates
         cxx_defaulted_functions
         cxx_nullptr
         cxx_trailing_return_types
         cxx_return_type_deduction)

if(BUILD_SHARED_LIBS)
  set_property(TARGET sqlite PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

set_target_properties(sqlite PROPERTIES FOLDER "dep")

add_library(SQLite::SQLite3 ALIAS sqlite)
