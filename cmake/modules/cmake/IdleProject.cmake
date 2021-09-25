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

include(IdleAddPlugin)
include(IdleAddSimplePlugin)

# Configure precompiled headers when both given files are present
function(idle_project_add_pch PCH_HEADER_FILE PCH_SOURCE_FILE)
  if(EXISTS "${PCH_HEADER_FILE}" AND EXISTS "${PCH_SOURCE_FILE}")
    message(STATUS "idle: Configuring pch for reuse")
    idle_add_plugin(pch TARGET_TYPE STATIC NO_INSTALL_TARGET SOURCES
                    ${PCH_SOURCE_FILE})
    target_precompile_headers(pch PRIVATE "${PROJECT_SOURCE_DIR}/src/pch.hpp")
    # target_compile_features(pch PUBLIC cxx_std_14)

    set_property(TARGET pch PROPERTY POSITION_INDEPENDENT_CODE ON)

    set_target_properties(pch PROPERTIES FOLDER "pch")
  else()
    message(
      STATUS
        "idle: No pch created, consider to add '${PCH_HEADER_FILE}' "
        "and '${PCH_SOURCE_FILE}' to your project, to speed up your build...")
  endif()
endfunction()

function(_idle_project_gen_find_package_stub FIND_PACKAGE_DIR
         IDLE_CURRENT_PLUGIN PLUGIN_DIR)
  configure_file(
    "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates/FindPlugin.in.cmake"
    "${FIND_PACKAGE_DIR}/${IDLE_CURRENT_PLUGIN}-config.cmake")
endfunction()

function(idle_project_add_plugins PLUGIN_SOURCE_DIR)
  set(arg_opt)
  set(arg_single)
  set(arg_multi FOLDER)
  cmake_parse_arguments(IDLE_PROJECT_ADD_PLUGINS "${arg_opt}" "${arg_single}"
                        "${arg_multi}" ${ARGN})

  # Collect the names of all directories
  file(
    GLOB PLUGINS
    LIST_DIRECTORIES true
    ${PLUGIN_SOURCE_DIR}/*)

  # Make the order filesystem independent
  list(SORT PLUGINS)

  # Create find_package stubs for all plugins inside the directory
  foreach(PLUGIN_DIR IN LISTS PLUGINS)
    if(IS_DIRECTORY ${PLUGIN_DIR})
      get_filename_component(IDLE_CURRENT_PLUGIN ${PLUGIN_DIR} NAME_WE)

      set(${IDLE_CURRENT_PLUGIN}_DIR "${CMAKE_CURRENT_BINARY_DIR}/configs")

      _idle_project_gen_find_package_stub(
        "${CMAKE_CURRENT_BINARY_DIR}/configs" "${IDLE_CURRENT_PLUGIN}"
        "${PLUGIN_DIR}")
    endif()
  endforeach()

  # Create the actual targets through a call to find_package
  foreach(PLUGIN_DIR IN LISTS PLUGINS)
    if(IS_DIRECTORY ${PLUGIN_DIR})
      get_filename_component(IDLE_CURRENT_PLUGIN ${PLUGIN_DIR} NAME_WE)
      find_package(${IDLE_CURRENT_PLUGIN} REQUIRED)

      if(IDLE_PROJECT_ADD_PLUGINS_FOLDER)
        set_target_properties(
          ${IDLE_CURRENT_PLUGIN}
          PROPERTIES FOLDER "${IDLE_PROJECT_ADD_PLUGINS_FOLDER}")
      endif()
    endif()
  endforeach()
endfunction()

function(idle_project_add_target_polish PLUGIN_SOURCE_DIR)
  if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    return()
  endif()

  if(NOT TARGET idle::polish)
    return()
  endif()

  if(TARGET POLISH)
    return()
  endif()

  file(
    GLOB PLUGINS
    LIST_DIRECTORIES true
    ${PLUGIN_SOURCE_DIR}/*)

  # Make the order filesystem independent
  list(SORT PLUGINS)

  set(POLISH_ARGS)
  foreach(PLUGIN_DIR IN LISTS PLUGINS)
    if(IS_DIRECTORY "${PLUGIN_DIR}/include")
      list(APPEND POLISH_ARGS -I "${PLUGIN_DIR}/include")
    endif()
    if(IS_DIRECTORY "${PLUGIN_DIR}/src")
      list(APPEND POLISH_ARGS -S "${PLUGIN_DIR}/src")
    endif()
  endforeach()

  if(POLISH_ARGS)
    add_custom_target(
      POLISH
      COMMAND $<TARGET_FILE:idle::polish> #
              ${POLISH_ARGS}
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
endfunction()

function(idle_project_add_target_start CONFIG_FILE)
  if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    return()
  endif()

  if(TARGET START)
    return()
  endif()

  if("${CMAKE_GENERATOR}" MATCHES "Visual Studio")
    add_custom_target(START)
    set_target_properties(
      START
      PROPERTIES VS_DEBUGGER_COMMAND "$<TARGET_FILE:idle::cli>" #
                 VS_DEBUGGER_COMMAND_ARGUMENTS "${CONFIG_FILE}" #
                 VS_DEBUGGER_WORKING_DIRECTORY "$(ProjectDir)")

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                 PROPERTY VS_STARTUP_PROJECT START)
  else()
    add_custom_target(
      START
      COMMAND "$<TARGET_FILE:idle::cli>" "${CONFIG_FILE}"
      USES_TERMINAL
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
  endif()
endfunction()
