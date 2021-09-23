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

# Adds the given arguments to the CMake (cache) variable
macro(_idle_add_flags VAR)
  foreach(FLAG ${ARGN})
    string(FIND "${${VAR}}" "${FLAG}" FLAG_FOUND)

    if("${FLAG_FOUND}" STREQUAL "-1")
      # message(STATUS "Adding '${FLAG}' to ${VAR}")

      set(${VAR} "${${VAR}} ${FLAG}")

      list(APPEND IDLE_ADDITIONAL_${VAR} "${FLAG}")

      if(IDLE_ADDITIONAL_${VAR}_STR)
        set(IDLE_ADDITIONAL_${VAR}_STR "${IDLE_ADDITIONAL_${VAR}_STR} ${FLAG}")
      else()
        set(IDLE_ADDITIONAL_${VAR}_STR "${FLAG}")
      endif()
    endif()
  endforeach()
endmacro()

include(cmake/configure_compiler.cmake)
include(cmake/configure_bases.cmake)
