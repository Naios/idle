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

function(_idle_install_purge_ex EXCLUDE_FILES)
  cmake_policy(SET CMP0057 NEW)

  if(CMAKE_INSTALL_COMPONENT OR COMPONENT)
    return()
  endif()

  if(NOT CMAKE_INSTALL_PREFIX OR NOT IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}")
    message(WARNING "CMAKE_INSTALL_PREFIX (${CMAKE_INSTALL_PREFIX}) not set "
                    "correctly, will skip purging of old plugins!")
    return()
  endif()

  file(
    GLOB ALL_INSTALLED_ARTIFACTS
    LIST_DIRECTORIES false
    "${CMAKE_INSTALL_PREFIX}/*.dll" #
    "${CMAKE_INSTALL_PREFIX}/*.so" #
    "${CMAKE_INSTALL_PREFIX}/*.dylib" #
    "${CMAKE_INSTALL_PREFIX}/*.lib" #
    "${CMAKE_INSTALL_PREFIX}/*.pdb")

  foreach(INSTALLED_ARTIFACT IN LISTS ALL_INSTALLED_ARTIFACTS)
    if(NOT "${INSTALLED_ARTIFACT}" IN_LIST EXCLUDE_FILES)
      message(STATUS "Uninstalling: '${INSTALLED_ARTIFACT}'")
      file(REMOVE "${INSTALLED_ARTIFACT}")
    endif()
  endforeach()
endfunction()

# Remove targets on install, that are not in the exclude list
function(idle_install_purge)
  if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    return()
  endif()

  install(
    CODE "
      include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")

      _idle_install_purge_ex(\"\${CMAKE_INSTALL_MANIFEST_FILES}\")
    "
    COMPONENT install-purge)
endfunction()
