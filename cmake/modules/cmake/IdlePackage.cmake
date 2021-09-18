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

include(IdleDownload)
include(IdleArchive)

function(idle_package_cache_dir OUT_CACHE_DIR)
  if(IDLE_PACKAGE_CACHE)
    set(${OUT_CACHE_DIR}
        "${IDLE_PACKAGE_CACHE}"
        PARENT_SCOPE)
  elseif(NOT "$ENV{IDLE_PACKAGE_CACHE}" STREQUAL "")
    file(TO_CMAKE_PATH "$ENV{IDLE_PACKAGE_CACHE}" IDLE_PACKAGE_CACHE_NORMALIZED)

    set(${OUT_CACHE_DIR}
        "${IDLE_PACKAGE_CACHE_NORMALIZED}"
        PARENT_SCOPE)
  else()
    set(${OUT_CACHE_DIR}
        "${CMAKE_BINARY_DIR}/package_cache"
        PARENT_SCOPE)
  endif()
endfunction()

function(idle_license_install_dir OUT_CACHE_DIR)
  set(${OUT_CACHE_DIR}
      "doc/license"
      PARENT_SCOPE)
endfunction()

function(_idle_package_download_url OUT_PACKAGE_URL PACKAGE_USER PACKAGE_NAME
         PACKAGE_VERSION)
  set(${OUT_PACKAGE_URL}
      "https://github.com/${PACKAGE_USER}/${PACKAGE_NAME}/archive/${PACKAGE_VERSION}.zip"
      PARENT_SCOPE)
endfunction()

function(idle_package_info PACKAGE OUT_USER OUT_NAME OUT_VERSION)
  string(REGEX MATCHALL
               "(^[A-Za-z0-9_-]+)|(/[A-Za-z0-9_-]+)|(@[A-Za-z0-9_.-]+$)"
               MATCHED "${PACKAGE}")

  list(LENGTH MATCHED MATCHED_SIZE)

  list(GET MATCHED 0 PARSED_USER)
  list(GET MATCHED 1 PARSED_NAME)
  string(SUBSTRING "${PARSED_NAME}" 1 -1 PARSED_NAME)

  if(${MATCHED_SIZE} GREATER_EQUAL 3)
    list(GET MATCHED 2 PARSED_VERSION)
    string(SUBSTRING "${PARSED_VERSION}" 1 -1 PARSED_VERSION)
  else()
    set(PARSED_VERSION)
  endif()

  set(${OUT_USER}
      "${PARSED_USER}"
      PARENT_SCOPE)
  set(${OUT_NAME}
      "${PARSED_NAME}"
      PARENT_SCOPE)
  set(${OUT_VERSION}
      "${PARSED_VERSION}"
      PARENT_SCOPE)
endfunction()

# The patch file can be created through something like: 'diff -c . ../other >
# ../mydiff.patch'
function(_idle_apply_patch WORKING_DIRECTORY PATCH_FILE)
  if(NOT Patch_EXECUTABLE)
    find_package(Patch REQUIRED)
  endif()

  message(STATUS "Patching '${WORKING_DIRECTORY}'\n"
                 "    with '${PATCH_FILE}'...")

  execute_process(
    COMMAND "${Patch_EXECUTABLE}" "-p1" "-i" "${PATCH_FILE}"
    WORKING_DIRECTORY "${WORKING_DIRECTORY}"
    RESULT_VARIABLE PATCH_RESULT)

  if(NOT PATCH_RESULT EQUAL 0)
    message(
      FATAL_ERROR
        "Patching of '${WORKING_DIRECTORY}' has failed! "
        "  With command: ${Patch_EXECUTABLE} -p1 -i ${PATCH_FILE} "
        "  See the log for details.")
  endif()
endfunction()

function(_idle_remove_directory DIR)
  if(EXISTS "${DIR}")
    if(IS_DIRECTORY "${DIR}")
      message(STATUS "Removing directory '${DIR}'")
      file(REMOVE_RECURSE "${DIR}")
    endif()
  endif()
endfunction()

function(_idle_write_protect_directory PROTECT_DIRECTORY)
  if(NOT IS_DIRECTORY "${PROTECT_DIRECTORY}")
    message(FATAL_ERROR "Something went wrong here, while trying to "
                        "write protect path '${PROTECT_DIRECTORY}'!")
  endif()

  message(STATUS "Write protecting directory '${PROTECT_DIRECTORY}'")

  if(WIN32)
    execute_process(
      COMMAND "attrib" "+R" "/L" "/S" "/D"
      WORKING_DIRECTORY "${PROTECT_DIRECTORY}"
      RESULT_VARIABLE WRITE_PROTECT_RESULT)
  else()
    message("TODO: Implement write protection for this platform")
    set(WRITE_PROTECT_RESULT 0)
  endif()

  if(NOT WRITE_PROTECT_RESULT EQUAL 0)
    message(
      FATAL_ERROR "Write protecting of '${PROTECT_DIRECTORY}' has failed! "
                  "See the log for details.")
  endif()
endfunction()

function(_idle_directory_commit FROM TO)
  # Protect the directory against unintended file changes
  _idle_write_protect_directory("${FROM}")

  # Commit the prepared archive to the registry
  message(STATUS "Committing directory '${TO}' to registry")

  if(NOT EXISTS "${TO}")
    file(RENAME "${FROM}" "${TO}")
  else()
    message(
      FATAL_ERROR
        "Ups, did multiple processes write to the "
        "same registry concurrently? Directory '${TO}' " "should not exist!")
  endif()
endfunction()

function(_idle_package_pull_ex PACKAGE_USER PACKAGE_NAME PACKAGE_VERSION)
  set(arg_opt NO_LICENSE_FILE)
  set(arg_single INTO URL SHA512 LICENSE_FILE BASE_DIR ARCHIVE_EXTENSION)
  set(arg_multi FILTER RENAME PATCH)
  cmake_parse_arguments(IDLE_PACKAGE_PULL "${arg_opt}" "${arg_single}"
                        "${arg_multi}" ${ARGN})

  idle_package_cache_dir(PACKAGE_CACHE_DIR)

  # Setup a default github assets pull URL
  if(NOT IDLE_PACKAGE_PULL_URL)
    _idle_package_download_url(IDLE_PACKAGE_PULL_URL "${PACKAGE_USER}"
                               "${PACKAGE_NAME}" "${PACKAGE_VERSION}")
  endif()
  string(SHA1 IDLE_PACKAGE_PULL_URL_SHA1 "${IDLE_PACKAGE_PULL_URL}")

  if(NOT IDLE_PACKAGE_PULL_ARCHIVE_EXTENSION)
    get_filename_component(IDLE_PACKAGE_PULL_ARCHIVE_EXTENSION
                           "${IDLE_PACKAGE_PULL_URL}" LAST_EXT)
  endif()

  # Setup the default github zip archive basedir (archive root dir)
  if(NOT DEFINED IDLE_PACKAGE_PULL_BASE_DIR)
    set(IDLE_PACKAGE_PULL_BASE_DIR "${PACKAGE_NAME}-${PACKAGE_VERSION}")
  endif()

  set(PATCH_CHECKSUMS)
  foreach(PATCH_ENTRY IN LISTS IDLE_PACKAGE_PULL_PATCH)
    if(NOT EXISTS "${PATCH_ENTRY}")
      message(FATAL_ERROR "Patch file ${PATCH_ENTRY} does not exist!")
    endif()

    file(MD5 "${PATCH_ENTRY}" PATCH_ENTRY_CHECKSUMS)
    list(APPEND PATCH_CHECKSUMS "${PATCH_ENTRY_CHECKSUMS}")
  endforeach()

  string(
    SHA1 PACKAGE_LOCATION_HASH
         "${IDLE_PACKAGE_PULL_URL}${IDLE_PACKAGE_PULL_BASE_DIR}/${IDLE_PACKAGE_PULL_FILTER}${IDLE_PACKAGE_PULL_LICENSE_FILE}${IDLE_PACKAGE_PULL_RENAME}"
  )

  # Build the hash here which contains all unique operations allpied to the
  # extracted archive
  string(
    SHA1 PACKAGE_LOCATION_HASH
         "${IDLE_PACKAGE_PULL_URL}${IDLE_PACKAGE_PULL_BASE_DIR}/${IDLE_PACKAGE_PULL_FILTER}${IDLE_PACKAGE_PULL_LICENSE_FILE}${IDLE_PACKAGE_PULL_RENAME}${PATCH_CHECKSUMS}"
  )

  set(PACKAGE_ROOT_LOCATION
      "${PACKAGE_CACHE_DIR}/packages/${PACKAGE_LOCATION_HASH}")
  set(PACKAGE_LOCATION "${PACKAGE_ROOT_LOCATION}/${PACKAGE_NAME}")
  if(NOT EXISTS "${PACKAGE_LOCATION}")
    # Download the package
    if(NOT IDLE_PACKAGE_PULL_URL)
      _idle_package_download_url(IDLE_PACKAGE_PULL_URL "${PACKAGE_USER}"
                                 "${PACKAGE_NAME}" "${PACKAGE_VERSION}")
    endif()

    set(PACKAGE_DOWNLOAD_FILENAME
        "${IDLE_PACKAGE_PULL_URL_SHA1}${IDLE_PACKAGE_PULL_ARCHIVE_EXTENSION}")
    set(PACKAGE_DOWNLOAD_LOCATION
        "${PACKAGE_CACHE_DIR}/downloads/${PACKAGE_DOWNLOAD_FILENAME}")
    if(EXISTS "${PACKAGE_DOWNLOAD_LOCATION}")
      _idle_verify_checksum(
        "${PACKAGE_DOWNLOAD_LOCATION}" "${IDLE_PACKAGE_PULL_URL}"
        "${IDLE_PACKAGE_PULL_SHA512}")
    else()
      idle_download("${IDLE_PACKAGE_PULL_URL}" "${PACKAGE_DOWNLOAD_LOCATION}"
                    SHA512 "${IDLE_PACKAGE_PULL_SHA512}")
    endif()

    if(IDLE_PACKAGE_PULL_FILTER)
      set(ARCHIVE_FILTER)

      if(IDLE_PACKAGE_PULL_BASE_DIR)
        foreach(ARCHIVE_PATTERN IN LISTS IDLE_PACKAGE_PULL_FILTER
                                         IDLE_PACKAGE_PULL_LICENSE_FILE)
          list(APPEND ARCHIVE_FILTER
               "${IDLE_PACKAGE_PULL_BASE_DIR}/${ARCHIVE_PATTERN}")
        endforeach()
      else()
        list(APPEND ARCHIVE_FILTER ${IDLE_PACKAGE_PULL_FILTER}
             ${IDLE_PACKAGE_PULL_LICENSE_FILE})
      endif()
    endif()

    set(TEMP_ROOT_LOCATION
        "${PACKAGE_CACHE_DIR}/packages/tmp_${PACKAGE_LOCATION_HASH}")
    set(TEMP_LOCATION "${TEMP_ROOT_LOCATION}/${IDLE_PACKAGE_PULL_BASE_DIR}")

    # Setup a temporary extraction location, delete it if it exists.
    if(EXISTS "${TEMP_ROOT_LOCATION}")
      # This indicates that the previous registry transaction has failed.
      file(REMOVE_RECURSE "${TEMP_ROOT_LOCATION}")
    endif()

    # Extract the package
    idle_archive_extract("${PACKAGE_DOWNLOAD_LOCATION}" "${TEMP_ROOT_LOCATION}"
                         FILTER ${ARCHIVE_FILTER})

    if(NOT EXISTS "${TEMP_LOCATION}")
      message(
        FATAL_ERROR
          "Archive base directory '${TEMP_LOCATION}' doesn't exist! Make sure "
          "to specify the base directory by specifying BASE_DIR!")
    endif()

    if(IDLE_PACKAGE_PULL_RENAME)
      foreach(RENAME_ENTRY IN LISTS IDLE_PACKAGE_PULL_RENAME)
        string(REPLACE "=" ";" RENAME_ENTRY ${RENAME_ENTRY})
        list(GET RENAME_ENTRY 0 RENAME_FROM)
        set(RENAME_FROM "${TEMP_LOCATION}/${RENAME_FROM}")

        list(GET RENAME_ENTRY 1 RENAME_TO)
        set(RENAME_TO "${TEMP_LOCATION}/${RENAME_TO}")

        get_filename_component(RENAME_TO_DIR "${RENAME_TO}" DIRECTORY)
        if(RENAME_TO_DIR AND NOT EXISTS "${RENAME_TO_DIR}")
          message(STATUS "Creating directory '${RENAME_TO_DIR}'")
          file(MAKE_DIRECTORY "${RENAME_TO_DIR}")
        endif()

        message(STATUS "Renaming file '${RENAME_FROM}'\n"
                       "           to '${RENAME_TO}'")
        file(RENAME "${RENAME_FROM}" "${RENAME_TO}")
      endforeach()
    endif()

    if(IDLE_PACKAGE_PULL_PATCH)
      foreach(PATCH_ENTRY IN LISTS IDLE_PACKAGE_PULL_PATCH)
        _idle_apply_patch("${TEMP_LOCATION}" "${PATCH_ENTRY}")
      endforeach()
    endif()

    if(NOT IDLE_PACKAGE_PULL_BASE_DIR STREQUAL PACKAGE_NAME)
      message(STATUS "Renaming archive base directory\n"
                     "        from '${TEMP_LOCATION}'\n"
                     "        to   '${TEMP_ROOT_LOCATION}/${PACKAGE_NAME}'")

      file(RENAME "${TEMP_LOCATION}" "${TEMP_ROOT_LOCATION}/${PACKAGE_NAME}")
    endif()

    _idle_directory_commit("${TEMP_ROOT_LOCATION}" "${PACKAGE_ROOT_LOCATION}")
  endif()

  if(IDLE_PACKAGE_PULL_LICENSE_FILE)
    string(TOLOWER "${PACKAGE_NAME}" LICENSE_FILE_NAME)

    idle_license_install_dir(IDLE_LICENSE_INSTALL_DIR)

    install(
      FILES "${PACKAGE_LOCATION}/${IDLE_PACKAGE_PULL_LICENSE_FILE}"
      DESTINATION "${IDLE_LICENSE_INSTALL_DIR}"
      RENAME "license-${LICENSE_FILE_NAME}.txt")
  elseif(NOT IDLE_PACKAGE_PULL_NO_LICENSE_FILE)
    message(
      AUTHOR_WARNING
        "'${PACKAGE}' has no license file specified! "
        "Use idle_package_pull(... \"LICENSE_FILE LICENSE_FILE.txt\")!")
  endif()

  # Set the result variable
  if(IDLE_PACKAGE_PULL_INTO)
    set(${IDLE_PACKAGE_PULL_INTO}
        ${PACKAGE_LOCATION}
        PARENT_SCOPE)
  endif()
endfunction()

macro(idle_package_pull PACKAGE)
  idle_package_info("${PACKAGE}" PACKAGE_USER PACKAGE_NAME PACKAGE_VERSION)

  _idle_package_pull_ex("${PACKAGE_USER}" "${PACKAGE_NAME}"
                        "${PACKAGE_VERSION}" ${ARGN})
endmacro()

macro(idle_enable_packages)
  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/dep")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/dep")

    idle_package_cache_dir(PACKAGE_CACHE_DIR)

    if((NOT DEFINED IDLE_PACKAGE_CACHE) AND ("$ENV{IDLE_PACKAGE_CACHE}" STREQUAL
                                             ""))
      message(
        "I will use the build dir as cache directory for dependencies.\n"
        " - Enable a cross project build cache by defining IDLE_PACKAGE_CACHE "
        "as environment variable or CMake variable!")

      if(WIN32)
        message(
          " - 'C:/Users/$ENV{USERNAME}/AppData/Local/idle/cache' is recommended!"
        )
      else()
        message(" - '~/.idle/cache' is recommended!")
      endif()
    else()
      message(STATUS "Using '${PACKAGE_CACHE_DIR}' as package cache dir")
    endif()
  endif()
endmacro()
