
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

#ifndef IDLE_CORE_PARTS_LINK_HPP_INCLUDED
#define IDLE_CORE_PARTS_LINK_HPP_INCLUDED

#include <idle/core/detail/parts/uses_impl.hpp>
#include <idle/core/service.hpp>

namespace idle {
template <typename T>
class Link : protected detail::UseImport {
public:
  template <typename... Args>
  Link(Service& owner, Part& parent, Args&&... args)
    : UseImport(owner)
    , export_(Inheritance::weak(parent), std::forward<Args>(args)...) {}
  explicit Link(Service& owner)
    : Link(owner, owner.parent()) {}

protected:
  Export& used() noexcept override {
    return export_;
  }
  Export const& used() const noexcept override {
    return export_;
  }

private:
  detail::UsesExport<T> export_;
};

template <typename... T>
using Composition = Implements<Link<T>...>;
} // namespace idle

#endif // IDLE_CORE_PARTS_LINK_HPP_INCLUDED
