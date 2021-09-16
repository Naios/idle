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

include(IdlePackage)

add_library(
  demangle STATIC
  ${CMAKE_CURRENT_LIST_DIR}/demangle/include/llvm/Demangle/Compiler.h
  ${CMAKE_CURRENT_LIST_DIR}/demangle/include/llvm/Demangle/ItaniumDemangle.h
  ${CMAKE_CURRENT_LIST_DIR}/demangle/include/llvm/Demangle/MicrosoftDemangleNodes.h
  ${CMAKE_CURRENT_LIST_DIR}/demangle/include/llvm/Demangle/Utility.h
  ${CMAKE_CURRENT_LIST_DIR}/demangle/include/llvm/Demangle/Demangle.h
  ${CMAKE_CURRENT_LIST_DIR}/demangle/include/llvm/Demangle/MicrosoftDemangle.h
  ${CMAKE_CURRENT_LIST_DIR}/demangle/include/llvm/Demangle/StringView.h
  ${CMAKE_CURRENT_LIST_DIR}/demangle/lib/Demangle/ItaniumDemangle.cpp
  ${CMAKE_CURRENT_LIST_DIR}/demangle/lib/Demangle/MicrosoftDemangle.cpp
  ${CMAKE_CURRENT_LIST_DIR}/demangle/lib/Demangle/MicrosoftDemangleNodes.cpp)

target_include_directories(demangle
                           PUBLIC ${CMAKE_CURRENT_LIST_DIR}/demangle/include)

target_link_libraries(demangle PRIVATE idle-dep-base)

target_compile_features(
  demangle
  PUBLIC cxx_alias_templates
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

if(BUILD_SHARED_LIBS)
  set_property(TARGET demangle PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

idle_license_install_dir(IDLE_LICENSE_INSTALL_DIR)

install(
  FILES "${CMAKE_CURRENT_LIST_DIR}/demangle/LICENSE.TXT"
  DESTINATION "${IDLE_LICENSE_INSTALL_DIR}"
  RENAME "license-llvm.txt")

set_target_properties(demangle PROPERTIES FOLDER "dep")

add_library(llvm::Demangle ALIAS demangle)
