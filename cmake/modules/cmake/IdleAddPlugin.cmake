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

function(idle_target_compile_options PLUGIN_NAME)
  target_compile_options(
    ${PLUGIN_NAME}
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:
            /MP
            /permissive-
            /W4
            /bigobj
            /wd4251
            /wd4275
            /wd4190
            /wd4127
            /Zc:throwingNew>
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
            -Wall
            -pedantic
            -Wextra>)

  target_link_options(
    ${PLUGIN_NAME} PRIVATE $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:
    /DEBUG:FASTLINK /PDBALTPATH:$<TARGET_PDB_FILE_NAME:${PLUGIN_NAME}> >)

  target_compile_definitions(
    ${PLUGIN_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>: NOMINMAX VC_EXTRALEAN
                           WIN32_LEAN_AND_MEAN>)
endfunction()

function(idle_install_plugin PLUGIN_NAME)
  if(IDLE_PLUGIN_NO_INSTALL)
    return()
  endif()

  get_target_property(TARGET_TYPE ${PLUGIN_NAME} TYPE)
  if(TARGET_TYPE STREQUAL SHARED_LIBRARY)
    install(
      TARGETS ${PLUGIN_NAME}
      COMPONENT ${PLUGIN_NAME}
      ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}"
      LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}"
      RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}")

    if(WIN32 AND MSVC)
      install(
        FILES $<TARGET_PDB_FILE:${PLUGIN_NAME}>
        COMPONENT ${PLUGIN_NAME}
        CONFIGURATIONS "Debug" "RelWithDebInfo"
        DESTINATION "${CMAKE_INSTALL_PREFIX}"
        OPTIONAL)
    endif()
  endif()
endfunction()

function(idle_add_plugin PLUGIN_NAME)
  set(arg_opt)
  set(arg_single TARGET_TYPE)
  set(arg_multi SOURCES)
  cmake_parse_arguments(IDLE_ADD_PLUGIN "${arg_opt}" "${arg_single}"
                        "${arg_multi}" ${ARGN})

  if(NOT IDLE_ADD_PLUGIN_TARGET_TYPE)
    set(IDLE_ADD_PLUGIN_TARGET_TYPE SHARED)
  endif()

  add_library(${PLUGIN_NAME} ${IDLE_ADD_PLUGIN_TARGET_TYPE}
                             ${IDLE_ADD_PLUGIN_SOURCES})
  target_link_libraries(${PLUGIN_NAME} PUBLIC Threads::Threads idle::idle
                                              idle::locality-plugin)

  set_property(TARGET ${PLUGIN_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

  set_target_properties(${PLUGIN_NAME} PROPERTIES FOLDER "plugins")

  idle_target_compile_options(${PLUGIN_NAME})
endfunction()
