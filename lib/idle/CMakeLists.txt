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

find_package(Threads REQUIRED)
find_package(function2 REQUIRED)
find_package(continuable REQUIRED)
find_package(cpp17_headers REQUIRED)
find_package(fmt REQUIRED)
find_package(json REQUIRED)
find_package(toml11 REQUIRED)
find_package(mpark_variant REQUIRED)
find_package(concurrentqueue REQUIRED)
find_package(spdlog REQUIRED)
find_package(efsw REQUIRED)
find_package(LIEF REQUIRED)
find_package(replxx REQUIRED)
find_package(hat-trie REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(mbedtls REQUIRED)
find_package(Demangle REQUIRED)

include(IdleGroupSources)
include(IdlePackage)

set(VERSION_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/core/version_impl.cpp.cmake.in")
set(VERSION_TARGET "${CMAKE_BINARY_DIR}/version_impl.cpp")

string(REPLACE ";" " " IDLE_ADDITIONAL_C_FLAGS_STR "${IDLE_ADDITIONAL_C_FLAGS}")
string(REPLACE ";" " " IDLE_ADDITIONAL_CXX_FLAGS_STR "${IDLE_ADDITIONAL_CXX_FLAGS}")


add_custom_target(
  version ALL
  COMMAND
    "${CMAKE_COMMAND}" -DBINARY_DIR="${CMAKE_BINARY_DIR}"
    -DSOURCE_DIR="${CMAKE_SOURCE_DIR}" -DSOURCE_FILE="${VERSION_SOURCE}"
    -DTARGET_FILE="${VERSION_TARGET}" -P
    "${CMAKE_SOURCE_DIR}/cmake/macros/version.cmake" "${CMAKE_BINARY_DIR}"
  BYPRODUCTS "${VERSION_TARGET}"
  SOURCES "${VERSION_SOURCE}"
  DEPENDS "${VERSION_SOURCE}"
  WORKING_DIRECTORY "${CMAKE_PROJECT_DIR}")

set_target_properties(version PROPERTIES FOLDER "lib")

add_library(idle ${CMAKE_CURRENT_LIST_DIR}/pch.hpp
                 ${CMAKE_CURRENT_LIST_DIR}/pch.cpp ${VERSION_TARGET})

macro(requires REQUIRED_MODULE)
  get_filename_component(CURRENT_MODULE_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)
  string(TOUPPER "${REQUIRED_MODULE}" REQUIRED_MODULE_OPTION)

  if(NOT IDLE_WITH_MODULE_${REQUIRED_MODULE_OPTION})
    message(STATUS "Enabling idle::${REQUIRED_MODULE} implicitly required "
                   "by idle::${CURRENT_MODULE_NAME}.")
    set(IDLE_WITH_MODULE_${REQUIRED_MODULE_OPTION} ON)

    include("${CMAKE_CURRENT_SOURCE_DIR}/${REQUIRED_MODULE}/module.cmake"
            OPTIONAL)
  endif()
endmacro()

option(IDLE_WITH_INTERNAL_LOG
       "Enables the internal idle internal logging for optimized builds" OFF)
if(IDLE_WITH_INTERNAL_LOG)
  target_compile_definitions(idle PRIVATE IDLE_DETAIL_HAS_LOG_LEVEL=0)
endif()

file(
  GLOB DIRECTORIES
  LIST_DIRECTORIES true
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*")

foreach(MODULE ${DIRECTORIES})
  if(IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${MODULE}")
    string(TOUPPER "${MODULE}" MODULE_OPTION)

    option(IDLE_WITH_MODULE_${MODULE_OPTION}
           "Enable the idle ${MODULE} module to be built into libidle" ON)
  endif()
endforeach()

foreach(MODULE ${DIRECTORIES})
  if(IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${MODULE}")
    string(TOUPPER "${MODULE}" MODULE_OPTION)

    if(IDLE_WITH_MODULE_${MODULE_OPTION})
      include("${CMAKE_CURRENT_LIST_DIR}/${MODULE}/module.cmake" OPTIONAL)
    endif()
  endif()
endforeach()

foreach(MODULE ${DIRECTORIES})
  if(IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${MODULE}")
    string(TOUPPER "${MODULE}" MODULE_OPTION)

    if(IDLE_WITH_MODULE_${MODULE_OPTION})
      file(
        GLOB_RECURSE SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/include/idle/${MODULE}/*.hpp
        ${PROJECT_SOURCE_DIR}/include/idle/${MODULE}/*.inl
        ${PROJECT_SOURCE_DIR}/include/idle/${MODULE}/*.cpp
        ${PROJECT_SOURCE_DIR}/lib/idle/${MODULE}/*.hpp
        ${PROJECT_SOURCE_DIR}/lib/idle/${MODULE}/*.inl
        ${PROJECT_SOURCE_DIR}/lib/idle/${MODULE}/*.cmake
        ${PROJECT_SOURCE_DIR}/lib/idle/${MODULE}/*.cpp
        ${PROJECT_SOURCE_DIR}/lib/idle/${MODULE}/*.md)

      target_sources(idle PRIVATE ${SOURCES})
    endif()
  endif()
endforeach()

target_include_directories(
  idle
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_SOURCE_DIR}/lib)

add_dependencies(idle version)

target_link_libraries(
  idle
  PUBLIC function2::function2 continuable::continuable
         cpp17_headers::cpp17_headers fmt::fmt mpark_variant
  PRIVATE idle-lib-base
          Threads::Threads
          concurrentqueue::concurrentqueue
          llvm::Demangle
          Boost::asio
          Boost::filesystem
          Boost::graph
          efsw::efsw
          spdlog::spdlog
          LIEF::LIB_LIEF
          replxx
          hat-trie::hat-trie
          nlohmann::json
          toml11::toml11
          SQLite::SQLite3
          mbedtls::mbedtls
          $<$<PLATFORM_ID:Windows>:crypt32>
          $<$<PLATFORM_ID:Windows>:Dbghelp>)

target_compile_definitions(
  idle
  PUBLIC $<$<CXX_COMPILER_ID:MSVC>:_ENABLE_EXTENDED_ALIGNED_STORAGE=1>
  PRIVATE IDLE_DETAIL_HAS_VERSION)

target_compile_options(
  idle
  PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:
    $<$<VERSION_LESS:$<CXX_COMPILER_VERSION>,19.26>:/experimental:preprocessor>
    $<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,19.26>:/Zc:preprocessor>
    >)

target_compile_features(
  idle
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

target_link_options(
  idle PRIVATE $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:
  /PDBALTPATH:$<TARGET_PDB_FILE_NAME:idle> >)

if("${CMAKE_GENERATOR}" MATCHES "Visual Studio")
  target_compile_definitions(
    idle PRIVATE IDLE_DETAIL_BUILD_TYPE="$(ConfigurationName)")
else()
  target_compile_definitions(
    idle PRIVATE IDLE_DETAIL_BUILD_TYPE="${CMAKE_BUILD_TYPE}")
endif()

if(IDLE_WITH_COROUTINES)
  target_compile_options(
    idle PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/await>
                $<$<CXX_COMPILER_ID:Clang>:-fcoroutines-ts>)

  target_compile_definitions(idle
                             PUBLIC CONTINUABLE_WITH_EXPERIMENTAL_COROUTINE)
endif()

if(BUILD_SHARED_LIBS)
  target_compile_definitions(idle PUBLIC IDLE_HAS_DYNAMIC_LINKING)

  set_property(TARGET idle PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

if(WIN32)
  set_target_properties(idle PROPERTIES OUTPUT_NAME "libidle")
endif()

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/idle" DESTINATION "include")

install(
  TARGETS idle
  EXPORT "${PROJECT_NAME}Targets"
  INCLUDES
  DESTINATION include
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

if(WIN32)
  install(
    FILES $<TARGET_PDB_FILE:idle>
    DESTINATION bin
    OPTIONAL)
endif()

option(IDLE_WITH_PCH "Enable the precompiled header compilation" ON)
if(IDLE_WITH_PCH)
  message(STATUS "Enabling CMake PCH")
  target_precompile_headers(idle PRIVATE ${CMAKE_CURRENT_LIST_DIR}/pch.hpp)
endif()

idle_group_sources(${CMAKE_CURRENT_LIST_DIR} ${CMAKE_SOURCE_DIR}/include/idle)

set_target_properties(idle PROPERTIES FOLDER "lib")
