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

cmake_minimum_required(VERSION 3.17)
project(plugins LANGUAGES C CXX)

message(STATUS "Generating plugins of ${PROJECT_SOURCE_DIR}")

include(IdleProject)
include(IdlePackage)
include(IdleTargetUtil)
include(IdleInstallPurge)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(idle REQUIRED)

#[[ Resolves the link libraries of the given targets through find_package ]]
idle_find_link_libraries(idle::idle)

#[[ Enable the idle package system and allows to find packages placed in 'dep']]
idle_enable_packages()

#[[ Adds a reusable precompiled-header (pch) target if both files are present ]]
idle_project_add_pch("${PROJECT_SOURCE_DIR}/src/pch.hpp"
                     "${PROJECT_SOURCE_DIR}/src/pch.cpp")

#[[ Adds the plugins from the given directory ]]
idle_project_add_plugins("${PROJECT_SOURCE_DIR}/src")

#[[ Adds a 'polish' target that formats sources and fixes include guards ]]
idle_project_add_target_polish("${PROJECT_SOURCE_DIR}/src")

#[[ Adds a 'start' target that runs your plugins through the idle CLI ]]
idle_project_add_target_start("${PROJECT_SOURCE_DIR}/idle.toml")

#[[ Removes files from the install directory that are not in the install set]]
idle_install_purge()
