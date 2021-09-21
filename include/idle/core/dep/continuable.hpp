
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

#ifndef IDLE_CORE_DEP_CONTINUABLE_HPP_INCLUDED
#define IDLE_CORE_DEP_CONTINUABLE_HPP_INCLUDED

#include <idle/core/detail/async_finalizer.hpp>
//
#include <continuable/continuable.hpp>
#include <idle/core/api.hpp>
#include <idle/core/dep/function.hpp>

namespace cti {
IDLE_API_EXTERN_IF_NOT_IMPORTED template class IDLE_API_IMPORT_IF_IMPORTED
    continuable_base<detail::erasure::continuation<>, signature_arg_t<>>;

IDLE_API_EXTERN_IF_NOT_IMPORTED template class IDLE_API_IMPORT_IF_IMPORTED
    result<>;
} // namespace cti

namespace fu2 {
inline namespace abi_400 {
namespace detail {
IDLE_API_EXTERN_IF_NOT_IMPORTED template class IDLE_API_IMPORT_IF_IMPORTED
    function<config<true, false, cti::continuation_capacity<>>,
             property<true, false,
                      void(cti::promise_base<cti::detail::erasure::callback<>,
                                             cti::signature_arg_t<>>),
                      bool(cti::is_ready_arg_t) const,
                      cti::result<>(cti::unpack_arg_t)>>;
}
} // namespace abi_400
} // namespace fu2

namespace idle {
using cti::async;
using cti::async_on;
using cti::cancel;
using cti::continuable;
using cti::continuable_base;
using cti::empty_result;
using cti::exception_arg_t;
using cti::exception_t;
using cti::exceptional_result;
using cti::is_continuable;
using cti::loop;
using cti::loop_break;
using cti::loop_continue;
using cti::loop_result;
using cti::make_cancelling_continuable;
using cti::make_continuable;
using cti::make_exceptional_continuable;
using cti::make_ready_continuable;
using cti::make_result;
using cti::map_pack;
using cti::populate;
using cti::promise;
using cti::promise_base;
using cti::promisify;
using cti::range_loop;
using cti::recover;
using cti::result;
using cti::rethrow;
using cti::split;
using cti::traverse_pack;
using cti::when_all;
using cti::when_any;
using cti::when_seq;
using cti::work;
} // namespace idle

#endif // IDLE_CORE_DEP_CONTINUABLE_HPP_INCLUDED
