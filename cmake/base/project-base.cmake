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

add_library(idle-project-base INTERFACE)

target_compile_features(
  idle-project-base
  INTERFACE cxx_alias_templates
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

target_compile_definitions(
  idle-project-base
  INTERFACE $<$<PLATFORM_ID:Windows>:
            _WIN32_WINNT=0x0601
            # WINVER=0x0A00 # Win10 _WIN32_WINNT=0x0A00 # Win10
            WIN32_LEAN_AND_MEAN
            NOMINMAX>
            $<$<AND:$<PLATFORM_ID:Windows>,$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>>:
            _WIN64>)

target_compile_options(idle-project-base INTERFACE $<$<CXX_COMPILER_ID:MSVC>:
                                                   /MP>)

if(IDLE_ADDITIONAL_C_FLAGS)
  message(STATUS "Using additional C flags: ${IDLE_ADDITIONAL_C_FLAGS}")
  target_compile_options(idle-project-base INTERFACE ${IDLE_ADDITIONAL_C_FLAGS})
endif()

if(IDLE_ADDITIONAL_CXX_FLAGS)
  message(STATUS "Using additional CXX flags: ${IDLE_ADDITIONAL_CXX_FLAGS}")
  target_compile_options(idle-project-base
                         INTERFACE ${IDLE_ADDITIONAL_CXX_FLAGS})
endif()
