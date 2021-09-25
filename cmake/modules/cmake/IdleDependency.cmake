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
include(IdleTargetUtil)
include(GNUInstallDirs)

function(_idle_check_dependency_version LIBRARY_NAME PACKAGE_VERSION
         FIND_VERSION)

  if((NOT PACKAGE_VERSION) AND (NOT FIND_VERSION))
    message(
      FATAL_ERROR
        "You didn't specify any find version for 'find_package(${LIBRARY_NAME} ...)'! "
        "Make sure to provide either a pinned version in 'idle_dependency(\"${PACKAGE}\" \"<version>\")' or "
        "use find_package with an explicit requested version: 'find_package(${LIBRARY_NAME} 1.0.0)'. "
        "For version strings incompatible with CMake you can define the ${LIBRARY_NAME}_FIND_VERSION variable "
        "before calling 'find_package(${LIBRARY_NAME} ...)' such as 'set(${LIBRARY_NAME}_FIND_VERSION \"<version>\")'."
    )
  endif()

  if(FIND_VERSION)
    if(PACKAGE_VERSION AND (NOT PACKAGE_VERSION VERSION_EQUAL FIND_VERSION))
      message(
        FATAL_ERROR
          "Required a version for find_package(${LIBRARY_NAME} ${FIND_VERSION}) which is not "
          "compatible with the pinned idle_dependency(\"${PACKAGE}\") version!")
    endif()
  endif()
endfunction()

function(idle_dependency PACKAGE)
  set(arg_opt NO_LICENSE_FILE EXTERNAL NO_FIND_PACKAGE DRY_RUN)
  set(arg_single
      CD
      AS
      LICENSE_FILE
      SUBDIRECTORY
      URL
      SHA512
      BASE_DIR)
  set(arg_multi
      OPTIONS
      FILTER
      PATCH
      RENAME
      INSTALL_RUNTIME
      CONFIGURATIONS
      TARGETS
      HINTS)
  cmake_parse_arguments(IDLE_DEPENDENCY "${arg_opt}" "${arg_single}"
                        "${arg_multi}" ${ARGN})

  idle_package_info("${PACKAGE}" PACKAGE_USER PACKAGE_NAME PACKAGE_VERSION)

  if(IDLE_DEPENDENCY_AS)
    set(LIBRARY_NAME ${IDLE_DEPENDENCY_AS})
  else()
    set(LIBRARY_NAME ${PACKAGE_NAME})
  endif()

  if(TARGET ${LIBRARY_NAME})
    return()
  endif()

  idle_package_cache_dir(PACKAGE_CACHE_DIR)

  _idle_check_dependency_version("${LIBRARY_NAME}" "${PACKAGE_VERSION}"
                                 "${${LIBRARY_NAME}_FIND_VERSION}")

  if(${LIBRARY_NAME}_FIND_VERSION)
    set(PACKAGE_VERSION "${${LIBRARY_NAME}_FIND_VERSION}")
  endif()

  if(IDLE_DEPENDENCY_NO_LICENSE_FILE)
    set(IDLE_DEPENDENCY_NO_LICENSE_FILE NO_LICENSE_FILE)
  endif()

  _idle_package_pull_ex(
    "${PACKAGE_USER}"
    "${PACKAGE_NAME}"
    "${PACKAGE_VERSION}"
    INTO
    PACKAGE_LOCATION
    URL
    ${IDLE_DEPENDENCY_URL}
    SHA512
    ${IDLE_DEPENDENCY_SHA512}
    BASE_DIR
    ${IDLE_DEPENDENCY_BASE_DIR}
    LICENSE_FILE
    ${IDLE_DEPENDENCY_LICENSE_FILE}
    ${IDLE_DEPENDENCY_NO_LICENSE_FILE}
    RENAME
    ${IDLE_DEPENDENCY_RENAME}
    FILTER
    ${IDLE_DEPENDENCY_FILTER}
    PATCH
    ${IDLE_DEPENDENCY_PATCH})

  if(IDLE_DEPENDENCY_CD)
    set(PACKAGE_WORKING_DIRECTORY "${PACKAGE_LOCATION}/${IDLE_DEPENDENCY_CD}")
  else()
    set(PACKAGE_WORKING_DIRECTORY "${PACKAGE_LOCATION}")
  endif()

  if(IDLE_DEPENDENCY_EXTERNAL)
    set(PACKAGE_COMMAND_LINE_ARGS)

    if(CMAKE_C_COMPILER)
      list(APPEND PACKAGE_COMMAND_LINE_ARGS
           "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
    endif()
    if(CMAKE_CXX_COMPILER)
      list(APPEND PACKAGE_COMMAND_LINE_ARGS
           "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
    endif()

    # get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

    # ALways use the default CMake generator for now
    if(WIN32 AND MSVC)
      set(IS_MULTI_CONFIG ON)
    else()
      set(IS_MULTI_CONFIG OFF)
    endif()

    if(IS_MULTI_CONFIG)
      if(CMAKE_CONFIGURATION_TYPES)
        set(CONFIGURATION_TYPES_LIST ${CMAKE_CONFIGURATION_TYPES})
      else()
        # non multi config generator -> multi config generator
        set(CONFIGURATION_TYPES_LIST "Debug;Release;MinSizeRel;RelWithDebInfo")
      endif()

      if(IDLE_DEPENDENCY_CONFIGURATIONS)
        set(CURRENT_CONFIGURATION_TYPE)

        foreach(CURRENT_CONFIGURATION IN LISTS CONFIGURATION_TYPES_LIST)
          list(FIND IDLE_DEPENDENCY_CONFIGURATIONS "${CURRENT_CONFIGURATION}"
               FIND_RESULT)

          if(NOT FIND_RESULT EQUAL -1)
            list(APPEND CURRENT_CONFIGURATION_TYPES "${CURRENT_CONFIGURATION}")
          endif()
        endforeach()
      else()
        set(CURRENT_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES})
      endif()
    else()
      list(APPEND PACKAGE_COMMAND_LINE_ARGS
           "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

      set(CURRENT_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
    endif()

    if(NOT CURRENT_CONFIGURATION_TYPES)
      if(IS_MULTI_CONFIG)
        message(
          FATAL_ERROR
            "There are no configurations to build "
            "(GENERATOR_IS_MULTI_CONFIG=ON, "
            "CMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}, "
            "IDLE_DEPENDENCY_CONFIGURATIONS=${IDLE_DEPENDENCY_CONFIGURATIONS})!"
        )
      else()
        message(
          FATAL_ERROR
            "There are no configurations to build (IS_MULTI_CONFIG=OFF)!")
      endif()
    endif()

    foreach(CURRENT_OPTION IN LISTS IDLE_DEPENDENCY_OPTIONS)
      string(REPLACE "@" "\\;" CURRENT_OPTION "${CURRENT_OPTION}")
      list(APPEND PACKAGE_COMMAND_LINE_ARGS "-D${CURRENT_OPTION}")
    endforeach()

    string(
      SHA1 PACKAGE_COMMAND_LINE_ARGS_HASH
           "${PACKAGE_USER}@${PACKAGE_NAME}@${PACKAGE_VERSION}@${PACKAGE_COMMAND_LINE_ARGS}"
    )

    set(INSTALL_PACKAGE_LOCATION
        "${PACKAGE_CACHE_DIR}/installs/${PACKAGE_COMMAND_LINE_ARGS_HASH}")

    set(IDLE_COMMIT_FILENAME COMMITTED)
    if(NOT EXISTS "${INSTALL_PACKAGE_LOCATION}/${IDLE_COMMIT_FILENAME}")
      string(
        SHA1 BUILD_HASH
             "${PACKAGE_USER}@${PACKAGE_NAME}@${PACKAGE_VERSION}@${PACKAGE_COMMAND_LINE_ARGS}"
      )

      _idle_remove_directory("${INSTALL_PACKAGE_LOCATION}")

      set(BUILD_PACKAGE_LOCATION "${PACKAGE_CACHE_DIR}/build/${BUILD_HASH}")

      file(MAKE_DIRECTORY "${BUILD_PACKAGE_LOCATION}")

      # Unhashed args:
      if(CMAKE_MODULE_PATH)
        string(REPLACE ";" "\\;" CURRENT_MODULE_PATH "${CMAKE_MODULE_PATH}")

        list(APPEND PACKAGE_COMMAND_LINE_ARGS
             "-DCMAKE_MODULE_PATH=${CURRENT_MODULE_PATH}")
      endif()

      message(STATUS "Building ${PACKAGE}\n"
                     "     into '${BUILD_PACKAGE_LOCATION}'...")
      foreach(CURRENT_ARG IN LISTS PACKAGE_COMMAND_LINE_ARGS)
        message(STATUS "     with: ${CURRENT_ARG}")
      endforeach()

      if(IDLE_DEPENDENCY_DRY_RUN)
        message(FATAL_ERROR "Skipping CMake invocation (dry run)")
        return()
      endif()

      execute_process(
        COMMAND
          "${CMAKE_COMMAND}" "${PACKAGE_WORKING_DIRECTORY}"
          # "-G" "${CMAKE_GENERATOR}"
          "-DCMAKE_INSTALL_PREFIX=${INSTALL_PACKAGE_LOCATION}"
          ${CURRENT_BUILD_TYPE} ${PACKAGE_COMMAND_LINE_ARGS}
        WORKING_DIRECTORY "${BUILD_PACKAGE_LOCATION}"
        RESULT_VARIABLE PROCESS_RESULT)

      if(NOT PROCESS_RESULT EQUAL 0)
        message(FATAL_ERROR "CMake generation has failed!"
                            "See the log for details.")
      endif()

      if(IS_MULTI_CONFIG)
        message(STATUS "Installing configurations through multi-config "
                       "generator: ${CURRENT_CONFIGURATION_TYPES}")
      else()
        message(
          STATUS "Installing configuration: ${CURRENT_CONFIGURATION_TYPES}")
      endif()

      if(NOT IDLE_DEPENDENCY_TARGETS)
        if(MSVC)
          list(APPEND IDLE_DEPENDENCY_TARGETS "INSTALL")
        else()
          list(APPEND IDLE_DEPENDENCY_TARGETS "install")
        endif()
      endif()

      foreach(CURRENT_CONFIGURATION IN LISTS CURRENT_CONFIGURATION_TYPES)
        foreach(CURRENT_TARGET IN LISTS IDLE_DEPENDENCY_TARGETS)
          message(
            STATUS
              "Building target ${CURRENT_TARGET} of ${PACKAGE} (${CURRENT_CONFIGURATION})\n"
              "     into '${INSTALL_PACKAGE_LOCATION}'...")
          execute_process(
            COMMAND
              "${CMAKE_COMMAND}" "--build" "${BUILD_PACKAGE_LOCATION}"
              "--target" "${CURRENT_TARGET}" "--config"
              "${CURRENT_CONFIGURATION}"
            WORKING_DIRECTORY "${BUILD_PACKAGE_LOCATION}"
            RESULT_VARIABLE PROCESS_RESULT)

          if(NOT PROCESS_RESULT EQUAL 0)
            message(FATAL_ERROR "The build has failed! "
                                "See the log for details.")
          endif()
        endforeach()
      endforeach()

      message(
        STATUS "Committing directory '${INSTALL_PACKAGE_LOCATION}' to registry")
      file(TOUCH "${INSTALL_PACKAGE_LOCATION}/${IDLE_COMMIT_FILENAME}")
      _idle_write_protect_directory("${INSTALL_PACKAGE_LOCATION}")

      message(
        STATUS
          "Build directory '${BUILD_PACKAGE_LOCATION}'\n    was installed "
          "and is not needed anymore.\n    It is kept for caching reasons, "
          "feel free to delete it.")
    endif()

    set(FIND_HINTS)
    foreach(HINT IN LISTS IDLE_DEPENDENCY_HINTS)
      list(APPEND FIND_HINTS "${INSTALL_PACKAGE_LOCATION}/${HINT}")
    endforeach()

    set(${LIBRARY_NAME}_DIR "${INSTALL_PACKAGE_LOCATION}")
    set(${LIBRARY_NAME}_ROOT "${INSTALL_PACKAGE_LOCATION}")
    set(${LIBRARY_NAME}_FOUND ON)

    if(NOT IDLE_DEPENDENCY_NO_FIND_PACKAGE)
      list(APPEND CMAKE_MODULE_PATH "${INSTALL_PACKAGE_LOCATION}")

      find_package(
        ${LIBRARY_NAME}
        REQUIRED
        HINTS
        "${INSTALL_PACKAGE_LOCATION}"
        ${FIND_HINTS}
        PATHS
        "${INSTALL_PACKAGE_LOCATION}")
    endif()

  else() # NOT IDLE_DEPENDENCY_EXTERNAL
    cmake_policy(PUSH)
    cmake_policy(SET CMP0077 NEW)
    set(OLD_CMAKE_POLICY_DEFAULT_CMP0077 ${CMAKE_POLICY_DEFAULT_CMP0077})
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

    set(${LIBRARY_NAME}_DIR "${PACKAGE_LOCATION}")
    set(${LIBRARY_NAME}_ROOT "${PACKAGE_LOCATION}")
    set(${LIBRARY_NAME}_FOUND ON)

    # Set all CMake options that were specified
    foreach(CURRENT_OPTION IN LISTS IDLE_DEPENDENCY_OPTIONS)
      string(REPLACE "=" ";" CURRENT_OPTION ${CURRENT_OPTION})
      list(GET CURRENT_OPTION 0 OPTION_KEY)
      list(GET CURRENT_OPTION 1 OPTION_VALUE)
      string(REPLACE "@" "\\;" OPTION_VALUE "${OPTION_VALUE}")
      set(${OPTION_KEY} ${OPTION_VALUE})
    endforeach()

    if(IDLE_DEPENDENCY_SUBDIRECTORY)
      add_subdirectory("${IDLE_DEPENDENCY_SUBDIRECTORY}"
                       "${CMAKE_BINARY_DIR}/package_bin/${LIBRARY_NAME}")
    else()
      add_subdirectory(
        "${PACKAGE_WORKING_DIRECTORY}"
        "${CMAKE_BINARY_DIR}/package_bin/${LIBRARY_NAME}" EXCLUDE_FROM_ALL)
    endif()

    set(CMAKE_POLICY_DEFAULT_CMP0077 ${OLD_CMAKE_POLICY_DEFAULT_CMP0077})
    cmake_policy(POP)

    if(TARGET ${LIBRARY_NAME})
      get_target_property(LIBRARY_TARGET_TYPE ${LIBRARY_NAME} TYPE)
      if(NOT LIBRARY_TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
        set_target_properties(${LIBRARY_NAME} PROPERTIES FOLDER "dep")
      endif()
    endif()
  endif()

  if(IDLE_DEPENDENCY_INSTALL_RUNTIME)
    foreach(CURRENT_INSTALL_RUNTIME IN LISTS IDLE_DEPENDENCY_INSTALL_RUNTIME)
      if(TARGET "${CURRENT_INSTALL_RUNTIME}")
        get_target_property(TARGET_TYPE ${CURRENT_INSTALL_RUNTIME} TYPE)
        if(TARGET_TYPE STREQUAL SHARED_LIBRARY)
          install(FILES "$<TARGET_FILE:${CURRENT_INSTALL_RUNTIME}>"
                  DESTINATION "${CMAKE_INSTALL_PREFIX}")
        endif()
      endif()
    endforeach()
  endif()

  # Pass PACKAGE_DIR and PACKAGE_FOUND downwards
  set(${LIBRARY_NAME}_DIR
      "${${LIBRARY_NAME}_DIR}"
      PARENT_SCOPE)
  set(${LIBRARY_NAME}_ROOT
      "${${LIBRARY_NAME}_ROOT}"
      PARENT_SCOPE)
  set(${LIBRARY_NAME}_FOUND
      "${${LIBRARY_NAME}_FOUND}"
      PARENT_SCOPE)

  message("- ${LIBRARY_NAME}_DIR ${${LIBRARY_NAME}_DIR}")
  message("- ${LIBRARY_NAME}_ROOT ${${LIBRARY_NAME}_ROOT}")
  message("- ${LIBRARY_NAME}_FOUND ${${LIBRARY_NAME}_FOUND}")
endfunction()

function(idle_header_dependency PACKAGE)
  set(arg_opt INSTALL EXPORT NO_LICENSE_FILE)
  set(arg_single AS SHA512 LICENSE_FILE URL BASE_DIR)
  set(arg_multi
      INCLUDE_DIRECTORIES
      DEFINITIONS
      FEATURES
      FILTER
      RENAME
      LINK_LIBRARIES
      PATCH)
  cmake_parse_arguments(IDLE_HEADER_DEPENDENCY "${arg_opt}" "${arg_single}"
                        "${arg_multi}" ${ARGN})

  idle_package_info("${PACKAGE}" PACKAGE_USER PACKAGE_NAME PACKAGE_VERSION)

  if(IDLE_HEADER_DEPENDENCY_AS)
    idle_target_name("${IDLE_HEADER_DEPENDENCY_AS}" LIBRARY_NAME)
    idle_target_namespace("${IDLE_HEADER_DEPENDENCY_AS}" LIBRARY_NAMESPACE)
  else()
    set(LIBRARY_NAME "${PACKAGE_NAME}")
    set(LIBRARY_NAMESPACE "${PACKAGE_NAME}::")
  endif()

  set(TARGET_NAME "${LIBRARY_NAMESPACE}${LIBRARY_NAME}")

  if(TARGET ${TARGET_NAME})
    return()
  endif()

  idle_package_cache_dir(PACKAGE_CACHE_DIR)

  _idle_check_dependency_version("${LIBRARY_NAME}" "${PACKAGE_VERSION}"
                                 "${${LIBRARY_NAME}_FIND_VERSION}")

  if(${LIBRARY_NAME}_FIND_VERSION)
    set(PACKAGE_VERSION "${${LIBRARY_NAME}_FIND_VERSION}")
  endif()

  if(NOT IDLE_HEADER_DEPENDENCY_INCLUDE_DIRECTORIES)
    set(IDLE_HEADER_DEPENDENCY_INCLUDE_DIRECTORIES "include")
    list(APPEND IDLE_HEADER_DEPENDENCY_FILTER "include")
  endif()

  if(IDLE_HEADER_DEPENDENCY_NO_LICENSE_FILE)
    set(IDLE_HEADER_DEPENDENCY_NO_LICENSE_FILE NO_LICENSE_FILE)
  endif()

  _idle_package_pull_ex(
    "${PACKAGE_USER}"
    "${PACKAGE_NAME}"
    "${PACKAGE_VERSION}"
    INTO
    PACKAGE_LOCATION
    URL
    ${IDLE_HEADER_DEPENDENCY_URL}
    BASE_DIR
    ${IDLE_HEADER_DEPENDENCY_BASE_DIR}
    FILTER
    ${IDLE_HEADER_DEPENDENCY_FILTER}
    LICENSE_FILE
    ${IDLE_HEADER_DEPENDENCY_LICENSE_FILE}
    ${IDLE_HEADER_DEPENDENCY_NO_LICENSE_FILE}
    SHA512
    ${IDLE_HEADER_DEPENDENCY_SHA512}
    RENAME
    ${IDLE_HEADER_DEPENDENCY_RENAME}
    PATCH
    ${IDLE_HEADER_DEPENDENCY_PATCH})

  set(INCLUDE_DIRS)
  foreach(INCLUDE_DIR IN LISTS IDLE_HEADER_DEPENDENCY_INCLUDE_DIRECTORIES)
    list(APPEND INCLUDE_DIRS
         "$<BUILD_INTERFACE:${PACKAGE_LOCATION}/${INCLUDE_DIR}>"
         "$<INSTALL_INTERFACE:${INCLUDE_DIR}>")
  endforeach()

  add_library(${LIBRARY_NAME} INTERFACE)

  target_include_directories(${LIBRARY_NAME} INTERFACE ${INCLUDE_DIRS})

  if(IDLE_HEADER_DEPENDENCY_DEFINITIONS)
    target_compile_definitions(${LIBRARY_NAME}
                               INTERFACE ${IDLE_HEADER_DEPENDENCY_DEFINITIONS})
  endif()

  if(IDLE_HEADER_DEPENDENCY_FEATURES)
    target_compile_features(${LIBRARY_NAME}
                            INTERFACE ${IDLE_HEADER_DEPENDENCY_FEATURES})
  endif()

  if(IDLE_HEADER_DEPENDENCY_LINK_LIBRARIES)
    foreach(LINK_LIBRARY IN LISTS IDLE_HEADER_DEPENDENCY_LINK_LIBRARIES)
      idle_target_name("${LINK_LIBRARY}" FIND_LINK_LIBRARY)

      find_package(${FIND_LINK_LIBRARY} REQUIRED)
      target_link_libraries(${LIBRARY_NAME} INTERFACE ${LINK_LIBRARY})
    endforeach()
  endif()

  # TODO: Use CMAKE_INSTALL_INCLUDEDIR maybe
  if(IDLE_HEADER_DEPENDENCY_INSTALL OR IDLE_HEADER_DEPENDENCY_EXPORT)
    foreach(INCLUDE_DIR IN LISTS IDLE_HEADER_DEPENDENCY_INCLUDE_DIRECTORIES)
      install(DIRECTORY "${PACKAGE_LOCATION}/${INCLUDE_DIR}"
              DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endforeach()
  endif()

  if(IDLE_HEADER_DEPENDENCY_EXPORT)
    include(CMakePackageConfigHelpers)

    set(EXPORT_NAME "${LIBRARY_NAME}-export")

    install(TARGETS "${LIBRARY_NAME}" EXPORT ${EXPORT_NAME})

    install(
      EXPORT ${EXPORT_NAME}
      NAMESPACE ${LIBRARY_NAMESPACE}
      DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake"
      FILE "Find${LIBRARY_NAME}.cmake"
      EXPORT_LINK_INTERFACE_LIBRARIES)
  endif()

  if(NOT TARGET_NAME STREQUAL LIBRARY_NAME)
    add_library(${TARGET_NAME} ALIAS ${LIBRARY_NAME})
  endif()

  #[[if(NOT TARGET_NAME STREQUAL LIBRARY_NAME)
    add_library(${TARGET_NAME} INTERFACE IMPORTED GLOBAL)
    target_link_libraries(${TARGET_NAME} INTERFACE ${LIBRARY_NAME})
  endif()]]

  set(${LIBRARY_NAME}_DIR
      "${PACKAGE_LOCATION}"
      PARENT_SCOPE)
  set(${LIBRARY_NAME}_FOUND
      ON
      PARENT_SCOPE)
endfunction()
