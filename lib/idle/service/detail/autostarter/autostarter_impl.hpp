
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

#ifndef IDLE_SERVICE_DETAIL_AUTOSTARTER_AUTOSTARTER_IMPL_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_AUTOSTARTER_AUTOSTARTER_IMPL_HPP_INCLUDED

#include <idle/core/parts/dyn_dependencies.hpp>
#include <idle/core/util/functional.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/autostarted.hpp>
#include <idle/service/autostarter.hpp>

namespace idle {
class AutostarterImpl final : public Autostarter,
                              public Upcastable<AutostarterImpl> {
public:
  using Autostarter::Autostarter;

private:
  // Subscribe to everything, release everything immediately when requested
  DynDependencies<Autostarted, AutostarterImpl> autostarted_{
      *this,                                         //
      IDLE_STATIC_RETURN(ReleaseReply::Now),         //
      IDLE_STATIC_RETURN(UseReply::UseNowAndInsert), //
      [](AutostarterImpl&, Autostarted&) {
        return BitSet<DependenciesFlags>{DependenciesFlags::Weak};
      }};
};
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_AUTOSTARTER_AUTOSTARTER_IMPL_HPP_INCLUDED
