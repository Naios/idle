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

function(_idle_verify_checksum FILE_LOCATION ORIGIN SHA512_CHECKSUM)
  if(NOT SHA512_CHECKSUM STREQUAL "")
    file(SHA512 "${FILE_LOCATION}" FILE_CHECKSUM)
    if(FILE_CHECKSUM STREQUAL SHA512_CHECKSUM)
      message(STATUS "Verified SHA512 of '${FILE_LOCATION}'")
    else()
      message(
        FATAL_ERROR
          "Failed to verify SHA512 checksum of '${FILE_LOCATION}'!\n" #
          "  expected: '${SHA512_CHECKSUM}'\n" #
          "   but was: '${FILE_CHECKSUM}'")
    endif()

  else(NOT IDLE_NO_MISSING_CHECKSUM_WARNING)
    # ^ If you really like to disable this, or use 'cmake -Wno-dev'

    # TODO Re-enable this
    #[[
    message(
      AUTHOR_WARNING
        "No SHA512 checksum present to verify\n" #
        "  file '${FILE_LOCATION}'\n" #
        "  from '${ORIGIN}'\n" #
        "  I will continue without it but consider adding one for integrity reasons!")
        ]]
  endif()
endfunction()

function(idle_download DOWNLOAD_URL DOWNLOAD_LOCATION)
  set(arg_opt)
  set(arg_single SHA512)
  cmake_parse_arguments(IDLE_DOWNLOAD "${arg_opt}" "${arg_single}" "" ${ARGN})

  if(NOT EXISTS "${DOWNLOAD_LOCATION}")
    set(TEMP_DOWNLOAD_LOCATION "${DOWNLOAD_LOCATION}.tmp")
    if(EXISTS "${TEMP_DOWNLOAD_LOCATION}")
      file(REMOVE "${TEMP_DOWNLOAD_LOCATION}")
    endif()

    message(STATUS "Downloading '${DOWNLOAD_URL}'\n"
                   "         to '${DOWNLOAD_LOCATION}'...")

    file(
      DOWNLOAD "${DOWNLOAD_URL}" "${TEMP_DOWNLOAD_LOCATION}"
      SHOW_PROGRESS
      STATUS PACKAGE_FETCH_RESULT)

    list(GET PACKAGE_FETCH_RESULT 0 PACKAGE_FETCH_STATUS_CODE)
    if(NOT PACKAGE_FETCH_STATUS_CODE EQUAL 0)
      message(
        FATAL_ERROR
          "Failed to download '${DOWNLOAD_URL}' to '${DOWNLOAD_LOCATION}'!")
    endif()

    _idle_verify_checksum("${TEMP_DOWNLOAD_LOCATION}" "${DOWNLOAD_URL}"
                          "${IDLE_DOWNLOAD_SHA512}")

    file(RENAME "${TEMP_DOWNLOAD_LOCATION}" "${DOWNLOAD_LOCATION}")
  else()
    _idle_verify_checksum("${DOWNLOAD_LOCATION}" "${DOWNLOAD_URL}"
                          "${IDLE_DOWNLOAD_SHA512}")
  endif()
endfunction()
