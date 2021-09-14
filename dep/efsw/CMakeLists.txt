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

#[[
# This CMakeLists.txt file was derived from its original file at:
# https://github.com/SpartanJ/efsw/blob/master/CMakeLists.txt
# The original file was published with the following license:
#
# Copyright (c) 2020 Martín Lucas Golini
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# This software is a fork of the "simplefilewatcher" by James Wynn (james@jameswynn.com)
# http://code.google.com/p/simplefilewatcher/ also MIT licensed.
#]]

add_library(
  efsw STATIC
  ${efsw_DIR}/src/efsw/Debug.cpp
  ${efsw_DIR}/src/efsw/DirectorySnapshot.cpp
  ${efsw_DIR}/src/efsw/DirectorySnapshotDiff.cpp
  ${efsw_DIR}/src/efsw/DirWatcherGeneric.cpp
  ${efsw_DIR}/src/efsw/FileInfo.cpp
  ${efsw_DIR}/src/efsw/FileSystem.cpp
  ${efsw_DIR}/src/efsw/FileWatcher.cpp
  ${efsw_DIR}/src/efsw/FileWatcherCWrapper.cpp
  ${efsw_DIR}/src/efsw/FileWatcherGeneric.cpp
  ${efsw_DIR}/src/efsw/FileWatcherImpl.cpp
  ${efsw_DIR}/src/efsw/Log.cpp
  ${efsw_DIR}/src/efsw/Mutex.cpp
  ${efsw_DIR}/src/efsw/String.cpp
  ${efsw_DIR}/src/efsw/System.cpp
  ${efsw_DIR}/src/efsw/Thread.cpp
  ${efsw_DIR}/src/efsw/Watcher.cpp
  ${efsw_DIR}/src/efsw/WatcherGeneric.cpp)

if(WIN32)
  target_sources(
    efsw
    PRIVATE ${efsw_DIR}/src/efsw/platform/win/FileSystemImpl.cpp
            ${efsw_DIR}/src/efsw/platform/win/MutexImpl.cpp
            ${efsw_DIR}/src/efsw/platform/win/SystemImpl.cpp
            ${efsw_DIR}/src/efsw/platform/win/ThreadImpl.cpp)
else()
  target_sources(
    efsw
    PRIVATE ${efsw_DIR}/src/efsw/platform/posix/FileSystemImpl.cpp
            ${efsw_DIR}/src/efsw/platform/posix/MutexImpl.cpp
            ${efsw_DIR}/src/efsw/platform/posix/SystemImpl.cpp
            ${efsw_DIR}/src/efsw/platform/posix/ThreadImpl.cpp)
endif()

if(WIN32)
  target_sources(efsw PRIVATE ${efsw_DIR}/src/efsw/WatcherWin32.cpp
                              ${efsw_DIR}/src/efsw/FileWatcherWin32.cpp)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  target_sources(efsw PRIVATE ${efsw_DIR}/src/efsw/FileWatcherInotify.cpp
                              ${efsw_DIR}/src/efsw/WatcherInotify.cpp)

  if(NOT EXISTS "/usr/include/sys/inotify.h"
     AND NOT EXISTS "/usr/local/include/sys/inotify.h")
    target_compile_definitions(efsw PRIVATE EFSW_INOTIFY_NOSYS)
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR APPLE)
  target_sources(efsw PRIVATE ${efsw_DIR}/src/efsw/FileWatcherKqueue.cpp
                              ${efsw_DIR}/src/efsw/WatcherKqueue.cpp)

  if(APPLE)
    target_sources(efsw PRIVATE ${efsw_DIR}/src/efsw/FileWatcherFSEvents.cpp
                                ${efsw_DIR}/src/efsw/WatcherFSEvents.cpp)

    exec_program(
      uname ARGS
      -v
      OUTPUT_VARIABLE OSX_VERSION)
    string(REGEX MATCH "[0-9]+" OSX_VERSION ${OSX_VERSION})
    if(NOT OSX_VERSION GREATER 9)
      target_compile_definitions(efsw PRIVATE EFSW_FSEVENTS_NOT_SUPPORTED)
    endif()

    target_link_libraries(efsw PUBLIC "-framework CoreFoundation"
                                      "-framework CoreServices")
  endif()
endif()

target_include_directories(
  efsw
  PUBLIC ${efsw_DIR}/include
  PRIVATE ${efsw_DIR}/src)

target_link_libraries(efsw PRIVATE idle-dep-base)

if(BUILD_SHARED_LIBS)
  set_property(TARGET efsw PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

set_target_properties(efsw PROPERTIES FOLDER "dep")

add_library(efsw::efsw ALIAS efsw)
