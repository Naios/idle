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

add_subdirectory(idle)
add_subdirectory(idle-locality-executable)
add_subdirectory(idle-locality-plugin)
# add_subdirectory(idle-rt)

include(ExternalProject)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

idle_license_install_dir(IDLE_LICENSE_INSTALL_DIR)

# Create an install target: Headers and license files
install(
  FILES "${PROJECT_SOURCE_DIR}/LICENSE.txt"
  DESTINATION "${IDLE_LICENSE_INSTALL_DIR}"
  RENAME "license-idle.txt")

install(FILES "${PROJECT_SOURCE_DIR}/Readme.md" DESTINATION "doc")

# Findidle.cmake
install(FILES "${PROJECT_SOURCE_DIR}/cmake/Findidle.cmake"
        DESTINATION "share/cmake")

# Config.cmake
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION "share/cmake/${PROJECT_NAME}")

# ConfigVersion.cmake
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/config.in.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "share/cmake/${PROJECT_NAME}"
  # PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION "share/cmake/${PROJECT_NAME}")

# Targets.cmake
install(
  EXPORT "${PROJECT_NAME}Targets"
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "share/cmake/${PROJECT_NAME}")

# CMake modules
install(DIRECTORY "${PROJECT_SOURCE_DIR}/cmake/modules/cmake"
        DESTINATION "share")
