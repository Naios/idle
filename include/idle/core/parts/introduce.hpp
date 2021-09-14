
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

#ifndef IDLE_CORE_PARTS_INTRODUCE_HPP_INCLUDED
#define IDLE_CORE_PARTS_INTRODUCE_HPP_INCLUDED

#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
/// Introduces a new service that depends on the current service into the system
template <typename Child>
class Introduce : public Export, public Inplace<Child> {
  static_assert(is_service<Child>::value,
                "Can only introduce a Service! "
                "If you are trying to depend on an Interface use "
                "idle::Dependency instead!");

public:
  template <typename... Args>
  explicit Introduce(Service& owner, Args&&... args)
    : Export(owner)
    , Inplace<Child>(Inheritance::weak(*static_cast<Export*>(this)),
                     std::forward<Args>(args)...)
    , owner_(owner) {

    // Set the internal lifetime to the one of the parent such that
    // we propagate persistent lifetimes.
    refSetInheritLifetime(Inplace<Child>::get(), owner);
  }

  void onPartInit() noexcept override {
    Inplace<Child>::get()->init();
  }
  void onPartDestroy() noexcept override {
    Inplace<Child>::get()->destroy();
  }

  explicit operator Child&() noexcept {
    return *Inplace<Child>::get();
  }

  Service& owner() noexcept override {
    return owner_;
  }
  Service const& owner() const noexcept override {
    return owner_;
  }

  Range<ChildrenList::iterator, ChildrenList::size_type>
  children() noexcept override {
    if (Inplace<Child>::get()->state().isInitializedUnsafe()) {
      return make_range(ChildrenList::iterator(*Inplace<Child>::get()),
                        ChildrenList::iterator{}, ChildrenList::size_type(1U));
    } else {
      return {};
    }
  }
  Range<ChildrenList::const_iterator, ChildrenList::size_type>
  children() const noexcept override {
    if (Inplace<Child>::get()->state().isInitializedUnsafe()) {
      return make_range(ChildrenList::const_iterator(*Inplace<Child>::get()),
                        ChildrenList::const_iterator{},
                        ChildrenList::size_type(1U));
    } else {
      return {};
    }
  }

private:
  Service& owner_;
};
} // namespace idle

#endif // IDLE_CORE_PARTS_INTRODUCE_HPP_INCLUDED
