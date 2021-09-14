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

add_executable(polish ${CMAKE_CURRENT_LIST_DIR}/main.cpp)

set_target_properties(polish PROPERTIES OUTPUT_NAME "idle-polish")

target_link_libraries(
  polish
  PRIVATE idle-lib-base
          Threads::Threads
          boost
          function2::function2
          continuable::continuable
          cpp17_headers::cpp17_headers
          fmt::fmt
          concurrentqueue::concurrentqueue)

target_compile_definitions(polish PRIVATE BOOST_NO_RTTI FMT_HEADER_ONLY)

set_target_properties(polish PROPERTIES FOLDER "tools" OUTPUT_NAME
                                                       "idle-polish")

set(IDLE_POLISH_ARGUMENTS
    -L
    "${PROJECT_SOURCE_DIR}/doc/license.cpp"
    -L
    "${PROJECT_SOURCE_DIR}/doc/license.cmake"
    -I
    "${PROJECT_SOURCE_DIR}/include"
    -S
    "${PROJECT_SOURCE_DIR}/lib"
    -S
    "${PROJECT_SOURCE_DIR}/tools"
    -S
    "${PROJECT_SOURCE_DIR}/examples"
    -I
    "${PROJECT_SOURCE_DIR}/test/testing/include"
    -S
    "${PROJECT_SOURCE_DIR}/test"
    -M
    "${PROJECT_SOURCE_DIR}/dep"
    -M
    "${PROJECT_SOURCE_DIR}/cmake")

string(REPLACE ";" " " IDLE_POLISH_ARGUMENTS_STR "${IDLE_POLISH_ARGUMENTS}")

set_target_properties(
  polish
  PROPERTIES VS_DEBUGGER_COMMAND "$<TARGET_FILE:polish>" #
             VS_DEBUGGER_COMMAND_ARGUMENTS "${IDLE_POLISH_ARGUMENTS_STR}" #
             VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

install(
  TARGETS polish
  INCLUDES
  DESTINATION "include"
  ARCHIVE DESTINATION "lib"
  LIBRARY DESTINATION "lib"
  RUNTIME DESTINATION "bin")

add_custom_target(
  run-polish
  COMMAND $<TARGET_FILE:polish> ${IDLE_POLISH_ARGUMENTS}
  DEPENDS polish
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

set_target_properties(run-polish PROPERTIES FOLDER "scripts")
