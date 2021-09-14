
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

#ifndef IDLE_CORE_ILIST_HPP_INCLUDED
#define IDLE_CORE_ILIST_HPP_INCLUDED

#include <idle/core/fwd.hpp>
#include <idle/core/util/intrusive_list.hpp>

namespace idle {
namespace tags {
/// Tags the intrusive list in which the parts of a service are stored
struct Parts {};
/// Tags the intrusive list in which the services
/// are stored inside the parent context.
struct Children {};
// Tags the intrusive list in which the dependents of a api are stored.
struct Export {};
/// Tags the intrusive list in which the users included in a context are exposed
struct Import {};
/// Tags the intrusive list in which the created
/// service of a interf::id are stored
struct Published {};
} // namespace tags

using PartList = intrusive_forward_list<Part, tags::Parts>;
using ChildrenList = intrusive_list<Service, tags::Children>;
using PublishedList = intrusive_list<Interface, tags::Published>;
using ImportList = intrusive_list<Usage, tags::Export>;
using ExportList = intrusive_list<Usage, tags::Import>;
} // namespace idle

#endif // IDLE_CORE_ILIST_HPP_INCLUDED
