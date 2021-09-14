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

include(IdleGroupSources)
include(IdleAddPlugin)

function(idle_add_simple_plugin PLUGIN_NAME PLUGIN_PATH)
  idle_add_plugin(${PLUGIN_NAME})

  file(
    GLOB_RECURSE SOURCES
    LIST_DIRECTORIES false
    ${PLUGIN_PATH}/*.h
    ${PLUGIN_PATH}/*.hh
    ${PLUGIN_PATH}/*.hpp
    ${PLUGIN_PATH}/*.inl
    ${PLUGIN_PATH}/*.cpp
    ${PLUGIN_PATH}/*.cc
    ${PLUGIN_PATH}/*.cxx)

  target_sources(${PLUGIN_NAME} PRIVATE ${SOURCES})

  if(TARGET pch)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM pch)
  endif()

  if(IS_DIRECTORY "${PLUGIN_PATH}/include")
    target_include_directories(${PLUGIN_NAME} PUBLIC "${PLUGIN_PATH}/include")
  endif()
  if(IS_DIRECTORY "${PLUGIN_PATH}/src")
    target_include_directories(${PLUGIN_NAME} PUBLIC "${PLUGIN_PATH}/src")
  endif()

  idle_group_sources("${PLUGIN_PATH}/src" "${PLUGIN_PATH}/include")

  idle_install_plugin(${PLUGIN_NAME})
endfunction()
