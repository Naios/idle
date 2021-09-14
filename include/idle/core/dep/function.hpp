
/*
 *   _____    _ _        .      .    .
 *  |_   _|  | | |  .       .           .
 *    | |  __| | | ___         .    .        .
 *    | | / _` | |/ _ \                .
 *   _| || (_| | |  __/ github.com/Naios/idle
 *  |_____\__,_|_|\___| AGPL v3 (Early Access)
 *
 * Copyright(c) 2018 - 2021 Denis Blank <denis.blank at outlook dot com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IDLE_CORE_DEP_FUNCTION_HPP_INCLUDED
#define IDLE_CORE_DEP_FUNCTION_HPP_INCLUDED

#include <function2/function2.hpp>
#include <idle/core/api.hpp>

namespace fu2 {
inline namespace abi_400 {
namespace detail {
extern template class IDLE_API_IMPORT_IF_IMPORTED function<
    config<true, true, capacity_default>, property<true, false, void()>>;

extern template class IDLE_API_IMPORT_IF_IMPORTED function<
    config<true, false, capacity_default>, property<true, false, void()>>;
} // namespace detail
} // namespace abi_400
} // namespace fu2

namespace idle {
using fu2::bad_function_call;
using fu2::function;
using fu2::function_view;
using fu2::overload;
using fu2::unique_function;
} // namespace idle

#endif // IDLE_CORE_DEP_FUNCTION_HPP_INCLUDED
