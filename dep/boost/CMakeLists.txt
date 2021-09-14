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

# find_package(OpenSSL REQUIRED)

if(UNIX)
  find_package(Backtrace REQUIRED)
endif()

file(GLOB_RECURSE FILESYSTEM_SRC "${Boost_DIR}/libs/filesystem/src/*.cpp")

add_library(Boost::headers INTERFACE IMPORTED GLOBAL)

add_library(boost STATIC ${CMAKE_CURRENT_LIST_DIR}/error_code.cpp
                         ${FILESYSTEM_SRC})

target_link_libraries(boost PUBLIC Boost::headers)

target_include_directories(
  boost
  PUBLIC ${Boost_DIR}/include
  PRIVATE ${Backtrace_INCLUDE_DIRS})

#[[
${Boost_DIR}boost/libs/algorithm/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/align/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/array/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/asio/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/assert/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/beast/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/bind/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/concept_check/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/config/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/container_hash/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/container/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/conversion/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/core/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/detail/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/dll/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/endian/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/filesystem/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/foreach/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/function_types/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/function/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/functional/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/fusion/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/graph/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/integer/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/intrusive/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/io/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/iterator/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/move/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/mp11/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/mpl/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/multi_index/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/optional/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/parameter/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/predef/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/preprocessor/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/process/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/property_map/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/range/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/smart_ptr/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/stacktrace/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/static_assert/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/system/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/throw_exception/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/tokenizer/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/tti/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/tuple/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/type_index/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/type_traits/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/typeof/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/unordered/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/utility/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/winapi/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/lexical_cast/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/numeric_conversion/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/math/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/any/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/spirit/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/xpressive/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/proto/include
    ${CMAKE_CURRENT_LIST_DIR}/boost/libs/exception/include
]]

target_link_libraries(
  boost
  PRIVATE idle-dep-base
  PUBLIC # OpenSSL is currently not needed, since we are not using the asio
         # network stack OpenSSL::SSL OpenSSL::Crypto Libraries for
         # boost::stacktrace
         $<$<PLATFORM_ID:Windows>:
         Dbgeng
         ole32
         >
         $<$<PLATFORM_ID:Linux>:
         ${Backtrace_LIBRARIES}
         ${CMAKE_DL_LIBS}
         >)

target_compile_features(
  boost
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

target_compile_definitions(
  boost
  PUBLIC BOOST_ALL_NO_LIB
         BOOST_ASIO_DISABLE_BOOST_DATE_TIME
         BOOST_ASIO_DISABLE_BOOST_REGEX
         BOOST_RANGE_ENABLE_CONCEPT_ASSERT=0
         BOOST_FILESYSTEM_NO_DEPRECATED
         BOOST_FILESYSTEM_NO_CXX20_ATOMIC_REF
         BOOST_MULTI_INDEX_DISABLE_SERIALIZATION
         # BOOST_ASIO_NO_DEPRECATED
         # $<$<PLATFORM_ID:Windows>:BOOST_STACKTRACE_USE_WINDBG>
         # $<$<PLATFORM_ID:Linux>:BOOST_STACKTRACE_USE_BACKTRACE>
)

if(BUILD_SHARED_LIBS)
  set_property(TARGET boost PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

set_target_properties(boost PROPERTIES FOLDER "dep")

foreach(BOOST_COMPONENT IN LISTS Boost_FIND_COMPONENTS)
  add_library(Boost::${BOOST_COMPONENT} ALIAS boost)
endforeach()
