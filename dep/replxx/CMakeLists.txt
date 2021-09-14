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

file(GLOB_RECURSE REPLXX_SRC "${replxx_DIR}/src/*.cpp"
     "${replxx_DIR}/src/*.cxx" "${replxx_DIR}/include/*.h"
     "${replxx_DIR}/src/*.hxx")

add_library(replxx STATIC ${REPLXX_SRC} "${replxx_DIR}/examples/util.c")

target_link_libraries(replxx PRIVATE idle-dep-base)

target_include_directories(
  replxx
  PUBLIC "${replxx_DIR}/include"
  PRIVATE "${replxx_DIR}/include/replxx")

target_compile_features(
  replxx
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

target_compile_definitions(replxx PUBLIC REPLXX_STATIC)

if(BUILD_SHARED_LIBS)
  set_property(TARGET replxx PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

set_target_properties(replxx PROPERTIES FOLDER "dep")
