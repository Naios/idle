
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

#include <idle/core/dep/continuable.hpp>

namespace cti {
template class IDLE_API_EXPORT
    continuable_base<detail::erasure::continuation<>, signature_arg_t<>>;

template class IDLE_API_EXPORT result<>;
} // namespace cti

namespace fu2 {
inline namespace abi_400 {
namespace detail {
template class IDLE_API_EXPORT
    function<config<true, false, cti::continuation_capacity<>>,
             property<true, false,
                      void(cti::promise_base<cti::detail::erasure::callback<>,
                                             cti::signature_arg_t<>>),
                      bool(cti::is_ready_arg_t) const,
                      cti::result<>(cti::unpack_arg_t)>>;
}
} // namespace abi_400
} // namespace fu2
