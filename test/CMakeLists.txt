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

find_package(Catch2 REQUIRED)

file(
  GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  LIST_DIRECTORIES false
  "${CMAKE_CURRENT_SOURCE_DIR}/testing/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/testing/*.hpp")
add_library(testing STATIC "${SOURCES}")
target_link_libraries(testing PUBLIC idle-project-base idle::idle
                                     Catch2::Catch2)
target_include_directories(testing
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/testing/include")
set_target_properties(testing PROPERTIES FOLDER "test")

# target_precompile_headers(testing PUBLIC
# "${CMAKE_CURRENT_SOURCE_DIR}/testing/include/testing/testing.hpp")

file(
  GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  LIST_DIRECTORIES false
  "${CMAKE_CURRENT_SOURCE_DIR}/idle/*.cpp")

set(TEST_TARGETS)
foreach(SOURCE IN LISTS SOURCES)
  get_filename_component(SOURCE_NAME ${SOURCE} NAME_WE)

  add_executable(${SOURCE_NAME} ${SOURCE})
  target_link_libraries(${SOURCE_NAME} PUBLIC testing)
  # target_precompile_headers(${SOURCE_NAME} REUSE_FROM testing)

  set_target_properties(${SOURCE_NAME} PROPERTIES FOLDER "test")

  add_test(NAME ${SOURCE_NAME} COMMAND ${SOURCE_NAME})
  list(APPEND TEST_TARGETS ${SOURCE_NAME})
endforeach()

add_custom_target(
  run-tests
  COMMAND ${CMAKE_CTEST_COMMAND}
  DEPENDS ${TEST_TARGETS})
set_target_properties(run-tests PROPERTIES FOLDER "scripts")
