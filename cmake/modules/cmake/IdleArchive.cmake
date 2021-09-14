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

function(idle_archive_extract EXTRACT_SOURCE_DIR EXTRACT_DEST_DIR)
  set(arg_opt)
  set(arg_single INTO URL SHA512)
  set(arg_multi FILTER)
  cmake_parse_arguments(IDLE_ARCHIVE_EXTRACT "${arg_opt}" "${arg_single}"
                        "${arg_multi}" ${ARGN})

  message(STATUS "Extracting '${EXTRACT_SOURCE_DIR}'\n"
                 "        to '${EXTRACT_DEST_DIR}'...")

  foreach(FILTER_ENTRY IN LISTS IDLE_ARCHIVE_EXTRACT_FILTER)
    message(STATUS "         * '${FILTER_ENTRY}'")
  endforeach()

  file(MAKE_DIRECTORY "${EXTRACT_DEST_DIR}")

  if(CMAKE_VERSION VERSION_LESS 3.18)
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E tar xzf "${EXTRACT_SOURCE_DIR}"
              ${IDLE_ARCHIVE_EXTRACT_FILTER}
      WORKING_DIRECTORY "${EXTRACT_DEST_DIR}"
      RESULT_VARIABLE PROCESS_RESULT)

    if(NOT PROCESS_RESULT EQUAL 0)
      message(FATAL_ERROR "Extraction of '${EXTRACT_SOURCE_DIR}' has failed! "
                          "See the log for details.")
    endif()
  else()
    file(
      ARCHIVE_EXTRACT
      INPUT
      "${EXTRACT_SOURCE_DIR}"
      DESTINATION
      "${EXTRACT_DEST_DIR}"
      PATTERNS
      ${IDLE_ARCHIVE_EXTRACT_FILTER})
  endif()
endfunction()
