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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/share/cmake")

macro(_idle_add_sanitizer SANITIZER SHORT)
  option(IDLE_WITH_${SHORT} "Enable clang ${SANITIZER} sanitizer" OFF)

  if(IDLE_WITH_${SHORT})
    if(IDLE_SANITIZER_SET)
      message(FATAL_ERROR "Setting more than one sanitizer is not supported!")
    endif()

    set(IDLE_SANITIZER_SET ON)

    list(APPEND IDLE_SANITIZER_FLAGS ${ARGN} #
         "-fno-omit-frame-pointer" #
         "-fno-optimize-sibling-calls")

    _idle_add_flags(CMAKE_C_FLAGS ${IDLE_SANITIZER_FLAGS})
    _idle_add_flags(CMAKE_CXX_FLAGS ${IDLE_SANITIZER_FLAGS})

    include(IdleDependency)

    # Install and use libcxx
    idle_dependency(
      "llvm/llvm-project@llvmorg-${CMAKE_CXX_COMPILER_VERSION}"
      FILTER
      "llvm"
      "libcxx"
      "libcxxabi"
      CD
      "llvm"
      TARGETS
      "cxx"
      "cxxabi"
      "install-cxx"
      "install-cxxabi"
      NO_LICENSE_FILE
      OPTIONS
      LLVM_USE_SANITIZER=${SANITIZER}
      LLVM_ENABLE_PROJECTS=libcxx@libcxxabi
      LIBCXX_INCLUDE_TEST=OFF
      LIBCXX_INCLUDE_BENCHMARKS=OFF
      EXTERNAL
      NO_FIND_PACKAGE)

    if(NOT (llvm-project_FOUND) OR NOT (llvm-project_DIR))
      message(
        FATAL_ERROR "Did not receive llvm-project_FOUND: ${llvm-project_FOUND} "
                    "or llvm-project_DIR: ${llvm-project_DIR}")
    endif()

    set(IDLE_LIBCXX_DIR "${llvm-project_DIR}")

    _idle_add_flags(CMAKE_C_FLAGS "-isystem"
                    "${IDLE_LIBCXX_DIR}/include/c++/v1" "-stdlib=libc++")
    _idle_add_flags(CMAKE_CXX_FLAGS "-isystem"
                    "${IDLE_LIBCXX_DIR}/include/c++/v1" "-stdlib=libc++")

    set(IDLE_SANITIZER_LINKER_FLAGS
        "-L ${IDLE_LIBCXX_DIR}/lib -l c++ -l c++abi")

    _idle_add_flags(CMAKE_EXE_LINKER_FLAGS ${IDLE_SANITIZER_LINKER_FLAGS})
    _idle_add_flags(CMAKE_SHARED_LINKER_FLAGS ${IDLE_SANITIZER_LINKER_FLAGS})

    list(APPEND CMAKE_INSTALL_RPATH "${IDLE_LIBCXX_DIR}/lib")
    list(APPEND CMAKE_BUILD_RPATH "${IDLE_LIBCXX_DIR}/lib")
  endif()
endmacro()

# Possible Sanitizers (LLVM_USE_SANITIZER): Address, Memory, MemoryWithOrigins,
# Undefined, Thread, DataFlow, and Address

# ASan falsely detects ODR violations from private symbols in shared libraries:
# https://github.com/google/sanitizers/issues/1017
# https://stackoverflow.com/questions/57390595/asan-detects-odr-violation-of-vtable-of-class-which-is-shared-with-dynamically-l
_idle_add_sanitizer("Address" ASAN "-fsanitize=address"
                    "-mllvm -asan-use-private-alias=1")

_idle_add_sanitizer("Thread" TSAN "-fsanitize=thread")
_idle_add_sanitizer("Memory" MSAN "-fsanitize=memory")
_idle_add_sanitizer("Undefined" UBSAN "-fsanitize=undefined")
