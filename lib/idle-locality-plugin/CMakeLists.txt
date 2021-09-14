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

add_library(idle::idle ALIAS idle)

add_library(locality-plugin STATIC
            ${CMAKE_CURRENT_LIST_DIR}/locality_plugin.cpp)

add_library(idle::locality-plugin ALIAS locality-plugin)

target_link_libraries(locality-plugin PUBLIC idle::idle)

# Hack to remove the link libraries from the exported dependencies
#[[set_target_properties(locality-plugin PROPERTIES INTERFACE_LINK_LIBRARIES
                                                 "idle::idle")
]]

target_compile_features(
  locality-plugin
  PRIVATE cxx_alias_templates
          cxx_auto_type
          cxx_constexpr
          cxx_decltype
          cxx_decltype_auto
          cxx_final
          cxx_lambdas
          cxx_generic_lambdas
          cxx_variadic_templates
          cxx_defaulted_functions
          cxx_nullptr
          cxx_trailing_return_types
          cxx_return_type_deduction)

target_compile_definitions(
  locality-plugin
  PRIVATE $<$<AND:$<PLATFORM_ID:Windows>,$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>>:
          _WIN64>)

target_compile_options(locality-plugin PRIVATE $<$<CXX_COMPILER_ID:MSVC>:
                                               /std:c++14 >)

if(BUILD_SHARED_LIBS)
  set_property(TARGET locality-plugin PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

if(WIN32)
  set_target_properties(
    locality-plugin PROPERTIES LIBRARY_OUTPUT_NAME "libidle-locality-plugin"
                               ARCHIVE_OUTPUT_NAME "libidle-locality-plugin")
else()
  set_target_properties(
    locality-plugin PROPERTIES LIBRARY_OUTPUT_NAME "idle-locality-plugin"
                               ARCHIVE_OUTPUT_NAME "idle-locality-plugin")
endif()

install(
  TARGETS locality-plugin
  EXPORT "${PROJECT_NAME}Targets"
  INCLUDES
  DESTINATION "include"
  ARCHIVE DESTINATION "lib"
  LIBRARY DESTINATION "lib"
  RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}")

set_target_properties(locality-plugin PROPERTIES FOLDER "lib")
