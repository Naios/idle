
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

#include <idle/core/util/assert.hpp>
#include <idle/service/detail/default_paths.hpp>
#include <idle/service/properties.hpp>
#include <idle/service/var.hpp>

namespace idle {
VarBase::VarBase(Service& owner, std::string key)
  : DynDependencyBase(owner)
  , key_(key.empty() ? detail::default_program_name() : std::move(key)) {

  IDLE_ASSERT(!key_.empty());
}

void VarBase::partName(std::ostream& os) const {
  print(os, FMT_STRING("idle::Var<{}>"), type());
}

void VarBase::onImportLock() noexcept {
  DynDependencyBase::onImportLock();
}

void VarBase::onImportUnlock() noexcept {
  DynDependencyBase::onImportUnlock();
}

Properties& VarBase::raw() noexcept {
  return cast<Properties>(DynDependencyBase::raw());
}

Properties const& VarBase::raw() const noexcept {
  return cast<Properties>(DynDependencyBase::raw());
}

Interface::Id VarBase::type() const noexcept {
  return Properties::id();
}

void VarBase::save(ConstReflectionPtr reflected) noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  raw().set(reflected, key());
}
} // namespace idle
