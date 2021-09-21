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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/share/cmake")

macro(_idle_add_sanitizer SANITIZER SHORT FLAG)
  option(IDLE_WITH_${SHORT} "Enable clang ${SANITIZER} sanitizer" OFF)

  if(IDLE_WITH_${SHORT})
    if(IDLE_SANITIZER_SET)
      message(FATAL_ERROR "Setting more than one sanitizer is not supported!")
    endif()

    set(IDLE_SANITIZER_SET ON)

    _idle_add_flag(CMAKE_C_FLAGS "${FLAG}")
    _idle_add_flag(CMAKE_CXX_FLAGS "${FLAG}")

    _idle_add_flag(CMAKE_C_FLAGS "-fno-omit-frame-pointer")
    _idle_add_flag(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer")

    _idle_add_flag(CMAKE_C_FLAGS "-fno-optimize-sibling-calls")
    _idle_add_flag(CMAKE_CXX_FLAGS "-fno-optimize-sibling-calls")
  endif()
endmacro()

# ASan falsely detects ODR violations from private symbols in shared libraries:
# https://github.com/google/sanitizers/issues/1017
# https://stackoverflow.com/questions/57390595/asan-detects-odr-violation-of-vtable-of-class-which-is-shared-with-dynamically-l
_idle_add_sanitizer("Address" ASAN
                    "-fsanitize=address -mllvm -asan-use-private-alias=1")

_idle_add_sanitizer("Thread" TSAN "-fsanitize=thread")
_idle_add_sanitizer("Memory" MSAN "-fsanitize=memory")
_idle_add_sanitizer("UndefinedBehavior" UBSAN "-fsanitize=undefined")
