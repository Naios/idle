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

find_package(mbedtls REQUIRED)
find_package(frozen REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(utfcpp REQUIRED)

set(LIEF_PE_SUPPORT 0)
set(LIEF_ELF_SUPPORT 0)
set(LIEF_MACHO_SUPPORT 0)

if(WIN32)
  set(BINARY_FORMAT "PE")
  set(LIEF_PE_SUPPORT 1)
elseif(APPLE)
  set(BINARY_FORMAT "MachO")
  set(LIEF_MACHO_SUPPORT 1)
else()
  set(BINARY_FORMAT "ELF")
  set(LIEF_ELF_SUPPORT 1)
endif()

file(
  GLOB LIEF_COMMON_SOURCES
  LIST_DIRECTORIES false
  "${LIEF_DIR}/src/*.cpp" #
  "${LIEF_DIR}/src/*.hpp" #
  "${LIEF_DIR}/src/*.tcc")

file(
  GLOB_RECURSE LIEF_SOURCES
  LIST_DIRECTORIES false
  "${LIEF_DIR}/include/LIEF/Abstract/*.hpp"
  "${LIEF_DIR}/include/LIEF/${BINARY_FORMAT}/*.hpp"
  "${LIEF_DIR}/include/LIEF/BinaryStream/*.hpp"
  "${LIEF_DIR}/src/Abstract/*.cpp"
  "${LIEF_DIR}/src/Abstract/*.hpp"
  "${LIEF_DIR}/src/Abstract/*.tcc"
  "${LIEF_DIR}/src/${BINARY_FORMAT}/*.cpp"
  "${LIEF_DIR}/src/${BINARY_FORMAT}/*.hpp"
  "${LIEF_DIR}/src/${BINARY_FORMAT}/*.tcc"
  "${LIEF_DIR}/src/BinaryStream/*.cpp"
  "${LIEF_DIR}/src/BinaryStream/*.hpp"
  "${LIEF_DIR}/src/BinaryStream/*.tcc"
  "${LIEF_DIR}/src/visitors/*.cpp"
  "${LIEF_DIR}/src/visitors/*.hpp")

file(
  GLOB LIEF_GENERATED_SOURCES
  RELATIVE "${LIEF_DIR}/include"
  LIST_DIRECTORIES false
  "${LIEF_DIR}/include/LIEF/*.in" #
  "${LIEF_DIR}/include/LIEF/${BINARY_FORMAT}/*.in")

set(LIEF_OAT_SUPPORT 0)
set(LIEF_DEX_SUPPORT 0)
set(LIEF_VDEX_SUPPORT 0)
set(LIEF_ART_SUPPORT 0)

set(LIEF_JSON_SUPPORT 1)
set(LIEF_LOGGING_SUPPORT 0)
set(LIEF_FROZEN_ENABLED 1)
set(LIEF_LOGGING_DEBUG_SUPPORT 0)

set(LIEF_SUPPORT_CXX11 1)
set(LIEF_SUPPORT_CXX14 1)
set(LIEF_SUPPORT_CXX17 0)

if(LIEF_ELF_SUPPORT)
  set(LIEF_ELF_ENUMS "#include \"LIEF/ELF/enums.inc\"")
  set(LIEF_ELF_STRUCTURES "#include \"LIEF/ELF/structures.inc\"")
endif()

if(LIEF_PE_SUPPORT)
  set(LIEF_PE_ENUMS "#include \"LIEF/PE/enums.inc\"")
  set(LIEF_PE_STRUCTURES "#include \"LIEF/PE/structures.inc\"")
endif()

if(LIEF_MACHO_SUPPORT)
  set(LIEF_MACHO_ENUMS "#include \"LIEF/MachO/enums.inc\"")
  set(LIEF_MACHO_STRUCTURES "#include \"LIEF/MachO/structures.inc\"")
endif()

foreach(LIEF_GENERATED_SOURCE ${LIEF_GENERATED_SOURCES})
  # https://stackoverflow.com/questions/30049180/strip-filename-shortest-extension-by-cmake-get-filename-removing-the-last-ext
  string(REGEX MATCH "^(.*)\\.[^.]*$" dummy ${LIEF_GENERATED_SOURCE})
  set(LIEF_GENERATED_SOURCE_NAME ${CMAKE_MATCH_1})

  configure_file(
    "${LIEF_DIR}/include/${LIEF_GENERATED_SOURCE}"
    "${CMAKE_CURRENT_BINARY_DIR}/include/${LIEF_GENERATED_SOURCE_NAME}" @ONLY)
endforeach()

add_library(lief STATIC ${LIEF_COMMON_SOURCES} ${LIEF_SOURCES})

target_include_directories(
  lief
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include ${LIEF_DIR}/include
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dep/rang/include
          ${CMAKE_CURRENT_LIST_DIR}/include ${LIEF_DIR}/src)

target_link_libraries(
  lief
  PRIVATE idle-dep-base
          mbedtls::mbedtls
          frozen::frozen
          nlohmann::json
          utfcpp::utfcpp
          Boost::leaf
          $<$<PLATFORM_ID:Windows>:ws2_32>
          spdlog::spdlog
          fmt::fmt)

target_compile_definitions(lief PRIVATE LIEF_STATIC)

target_compile_options(lief PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/FIiso646.h>)

if(BUILD_SHARED_LIBS)
  set_property(TARGET lief PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

set_target_properties(lief PROPERTIES FOLDER "dep")

add_library(LIEF::LIB_LIEF ALIAS lief)

idle_group_sources("${LIEF_DIR}/src" "${LIEF_DIR}/include/LIEF")
