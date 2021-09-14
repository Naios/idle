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

function(idle_target_name TARGET_NAMESPACE_NAME OUT_VAR)
  string(REPLACE "::" ";" TARGET_NAMESPACE_NAME "${TARGET_NAMESPACE_NAME}")
  list(REVERSE TARGET_NAMESPACE_NAME)
  list(GET TARGET_NAMESPACE_NAME 0 TARGET_NAMESPACE_NAME)
  set(${OUT_VAR}
      "${TARGET_NAMESPACE_NAME}"
      PARENT_SCOPE)
endfunction()

function(idle_target_namespace TARGET_NAMESPACE_NAME OUT_VAR)
  string(REPLACE "::" ";" TARGET_NAMESPACE_NAME "${TARGET_NAMESPACE_NAME}")
  list(LENGTH TARGET_NAMESPACE_NAME TARGET_NAMESPACE_NAME_LENGTH)

  if(TARGET_NAMESPACE_NAME_LENGTH GREATER 1)
    list(GET TARGET_NAMESPACE_NAME 0 TARGET_NAMESPACE_NAME)
    set(${OUT_VAR}
        "${TARGET_NAMESPACE_NAME}::"
        PARENT_SCOPE)
  else()
    set(${OUT_VAR}
        ""
        PARENT_SCOPE)
  endif()
endfunction()

function(idle_find_link_libraries TARGET)
  get_property(
    LINK_LIBRARIES_SET
    TARGET "${TARGET}"
    PROPERTY INTERFACE_LINK_LIBRARIES
    SET)

  if(NOT LINK_LIBRARIES_SET)
    return()
  endif()

  get_target_property(LINK_LIBRARIES "${TARGET}" INTERFACE_LINK_LIBRARIES)

  foreach(LINK_LIBRARY IN LISTS LINK_LIBRARIES)
    if(TARGET ${LINK_LIBRARY})
      continue()
    endif()

    idle_target_name("${LINK_LIBRARY}" LINK_LIBRARY_NAME)
    find_package(${LINK_LIBRARY_NAME} REQUIRED)
    idle_find_link_libraries(${LINK_LIBRARY})
  endforeach()
endfunction()
