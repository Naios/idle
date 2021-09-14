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

file(
  GLOB_RECURSE MBEDTLS_SOURCES
  LIST_DIRECTORIES false
  "${mbedtls_DIR}/library/*.c")

add_library(mbedtls STATIC ${MBEDTLS_SOURCES})

target_include_directories(mbedtls PUBLIC "${mbedtls_DIR}/include")

# LIEF requires this
target_compile_definitions(
  mbedtls PUBLIC MBEDTLS_MD2_C MBEDTLS_MD4_C
                 MBEDTLS_X509_ALLOW_UNSUPPORTED_CRITICAL_EXTENSION)

target_link_libraries(mbedtls PRIVATE idle-dep-base)

set_target_properties(mbedtls PROPERTIES FOLDER "dep")

if(BUILD_SHARED_LIBS)
  set_property(TARGET mbedtls PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

add_library(mbedtls::mbedtls ALIAS mbedtls)
