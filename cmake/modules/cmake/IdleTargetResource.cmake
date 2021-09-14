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

function(idle_target_resource TARGET SOURCE_FILE)
  set(arg_opt STATIC)
  set(arg_single AS TARGET_NAME)
  set(arg_multi)
  cmake_parse_arguments(IDLE_RESOURCE "${arg_opt}" "${arg_single}"
                        "${arg_multi}" ${ARGN})

  if(NOT IDLE_RESOURCE_AS)
    get_filename_component(IDLE_RESOURCE_AS "${SOURCE_FILE}" NAME_WE)
    string(MAKE_C_IDENTIFIER "${IDLE_RESOURCE_AS}" IDLE_RESOURCE_AS)
  endif()

  if(NOT IDLE_RESOURCE_TARGET_NAME)
    set(IDLE_RESOURCE_TARGET_NAME "${TARGET}_${IDLE_RESOURCE_AS}")
  endif()

  set(TARGET_FILE
      "${CMAKE_CURRENT_BINARY_DIR}/idle_res/${IDLE_RESOURCE_AS}.cpp")

  if(IDLE_RESOURCE_STATIC)
    include(
      "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates/IdleGenerateResource.cmake")
  else()
    add_custom_target(
      ${IDLE_RESOURCE_TARGET_NAME}
      COMMAND
        "${CMAKE_COMMAND}" -DBINARY_DIR="${CMAKE_BINARY_DIR}"
        -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
        -DIDLE_RESOURCE_AS="${IDLE_RESOURCE_AS}" -DSOURCE_FILE="${SOURCE_FILE}"
        -DTARGET_FILE="${TARGET_FILE}" -P
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates/IdleGenerateResource.cmake"
        "${CMAKE_BINARY_DIR}"
      BYPRODUCTS "${TARGET_FILE}"
      SOURCES "${SOURCE_FILE}"
      DEPENDS "${SOURCE_FILE}"
      WORKING_DIRECTORY "${CMAKE_PROJECT_DIR}")

    set_target_properties(${IDLE_RESOURCE_TARGET_NAME} PROPERTIES FOLDER "res")
    add_dependencies(${TARGET} ${IDLE_RESOURCE_TARGET_NAME})
  endif()

  target_sources(${TARGET} PRIVATE "${TARGET_FILE}")
endfunction()
