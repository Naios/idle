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

include(IdleDependency)

file(
  GLOB SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/*.hpp
  ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

# file( GLOB_RECURSE RESOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
# ${CMAKE_CURRENT_LIST_DIR}/install/*)

add_executable(
  cli ${SOURCES} ${RESOURCES} $<$<PLATFORM_ID:Windows>:
      ${CMAKE_CURRENT_LIST_DIR}/cli.rc ${CMAKE_CURRENT_LIST_DIR}/cli.ico >)

add_executable(idle::cli ALIAS cli)

get_filename_component(CLI_PARENT_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
target_include_directories(cli PRIVATE ${CLI_PARENT_DIR})

target_link_libraries(
  cli
  PRIVATE idle-lib-base boost
  PUBLIC idle locality-executable)

target_link_options(
  cli PRIVATE $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:
  /PDBALTPATH:$<TARGET_PDB_FILE_NAME:cli> >)

add_dependencies(cli locality-executable locality-plugin polish)

# Export the current workspace such that it can be consumed
set(WORKSPACE_EXPORT_DIR "${CMAKE_CURRENT_BINARY_DIR}/exports")
export(
  TARGETS idle locality-executable locality-plugin cli polish
  NAMESPACE idle::
  FILE "${WORKSPACE_EXPORT_DIR}/Findidle.cmake"
  EXPORT_LINK_INTERFACE_LIBRARIES)

get_target_property(IDLE_LINK_LIBRARIES idle INTERFACE_LINK_LIBRARIES)
foreach(IDLE_LINK_LIBRARY IN LISTS IDLE_LINK_LIBRARIES)
  idle_target_name("${IDLE_LINK_LIBRARY}" IDLE_LINK_LIBRARY_NAME)
  idle_target_namespace("${IDLE_LINK_LIBRARY}" IDLE_LINK_LIBRARY_NAMESPACE)

  export(
    TARGETS ${IDLE_LINK_LIBRARY_NAME}
    NAMESPACE ${IDLE_LINK_LIBRARY_NAMESPACE}
    FILE "${WORKSPACE_EXPORT_DIR}/Find${IDLE_LINK_LIBRARY_NAME}.cmake"
    EXPORT_LINK_INTERFACE_LIBRARIES)
endforeach()

add_custom_command(
  TARGET cli
  POST_BUILD DEPENDS ${RESOURCES}
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_LIST_DIR}/install"
          "$<TARGET_FILE_DIR:cli>/.."
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/cmake/modules"
    "$<TARGET_FILE_DIR:cli>/../share"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${WORKSPACE_EXPORT_DIR}"
          "$<TARGET_FILE_DIR:cli>/../share/cmake")

set_target_properties(cli PROPERTIES OUTPUT_NAME "idle")

idle_group_sources(${CMAKE_CURRENT_LIST_DIR})

install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/install/etc"
                  "${CMAKE_CURRENT_LIST_DIR}/install/project"
        DESTINATION "${CMAKE_INSTALL_PREFIX}")

install(
  FILES "${PROJECT_SOURCE_DIR}/.clang-format"
        "${PROJECT_SOURCE_DIR}/.clang-tidy"
        "${PROJECT_SOURCE_DIR}/.editorconfig"
        "${PROJECT_SOURCE_DIR}/.gitignore"
        "${PROJECT_SOURCE_DIR}/.gitattributes"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/project")

file(
  GLOB DIRECTORIES
  LIST_DIRECTORIES true
  RELATIVE "${CMAKE_SOURCE_DIR}/examples"
  CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/examples/*")

foreach(IDLE_CURRENT_EXAMPLE IN LISTS DIRECTORIES)
  set(IDLE_CURRENT_EXAMPLE
      "${CMAKE_SOURCE_DIR}/examples/${IDLE_CURRENT_EXAMPLE}")
  if(IS_DIRECTORY "${IDLE_CURRENT_EXAMPLE}")
    install(DIRECTORY "${IDLE_CURRENT_EXAMPLE}" DESTINATION "project/examples")
  endif()
endforeach()

install(
  TARGETS cli
  EXPORT "${PROJECT_NAME}Targets"
  INCLUDES
  DESTINATION "include"
  ARCHIVE DESTINATION "lib"
  LIBRARY DESTINATION "lib"
  RUNTIME DESTINATION "bin")

if(WIN32)
  install(
    FILES $<TARGET_PDB_FILE:cli>
    DESTINATION bin
    OPTIONAL)
endif()

set_target_properties(cli PROPERTIES FOLDER "tools")

set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT cli)
