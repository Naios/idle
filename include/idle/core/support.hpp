
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

#ifndef IDLE_CORE_SUPPORT_HPP_INCLUDED
#define IDLE_CORE_SUPPORT_HPP_INCLUDED

#include <iosfwd>
#include <type_traits>
#include <utility>
#include <idle/core/part.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/meta.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/core/util/type_name.hpp>

namespace idle {
/// Makes it possible to attach parts to a service through inheritance
///
/// This is a convenient helper that automatically passes the service
/// itself to all implemented parts and joins implemented non virtual
/// method with the service methods correctly.
///
/// Mainly this is used to inherit interfaces from the service but
/// it can also be useful to inherit importer or exporter parts.
template <typename Parent, typename... Parts>
class Extends;
template <typename Parent>
class Extends<Parent> : public Parent {
public:
  using Super = Extends;

  template <typename... Args>
  explicit Extends(Inheritance inh, Args&&...)
    : Parent(std::move(inh)) {}

  using Parent::refCounter;
  using Parent::weakRefCounter;
  using Parent::operator==;
  using Parent::operator!=;
  using Parent::epoch;
  using Parent::name;
};
template <typename Parent, typename Current, typename... Rest>
class Extends<Parent, Current, Rest...> : public Extends<Parent, Rest...>,
                                          public Current {
public:
  using Super = Extends;

  template <typename... Args>
  /*implicit*/ Extends(Inheritance parent, Args&&... args)
    : Extends<Parent, Rest...>(std::move(parent), args...)
    , Current(*static_cast<Parent*>(this), std::forward<Args>(args)...) {}

  Extends(Extends&&) = delete;
  Extends(Extends const&) = delete;
  Extends& operator=(Extends&&) = delete;
  Extends& operator=(Extends const&) = delete;

  Service& owner() noexcept override {
    return *this;
  }
  Service const& owner() const noexcept override {
    return *this;
  }

  using Current::classof;

  using Extends<Parent, Rest...>::refCounter;
  using Extends<Parent, Rest...>::weakRefCounter;
  using Extends<Parent, Rest...>::operator==;
  using Extends<Parent, Rest...>::operator!=;
  using Extends<Parent, Rest...>::name;
  using Extends<Parent, Rest...>::epoch;

  friend std::ostream& operator<<(std::ostream& os, Extends const& impl) {
    // TODO Is this really needed?
    return os << *static_cast<Parent const*>(&impl);
  }
};

/// Implements the given parts on a service class while
/// exposing the parts to everybody.
template <typename... Parts>
using Implements = Extends<Service, Parts...>;

template <typename Current, typename... Rest>
class DefaultService : public Implements<Current, Rest...> {
public:
  using Implements<Current, Rest...>::Implements;

protected:
  void name(std::ostream& os) const override {
    // TODO Maybe print a composition of all part names,
    // instead of the current one
    Current::partName(os);
  }
};
} // namespace idle

/// Embeds useful information into a idle::part class, for example
/// the actual class name without the usage of RTTI while also hiding
/// any potential implementation defined class names.
#define IDLE_PART                                                              \
protected:                                                                     \
  void partName(std::ostream& os) const override {                             \
    static constexpr auto                                                      \
        type = type_name<unrefcv_t<std::remove_pointer_t<decltype(this)>>>();  \
    os << ::idle::StringView(type);                                            \
  }

/// Embeds useful information into a idle::service class, for example
/// the actual class name without the usage of RTTI while also hiding
/// any potential implementation defined class names.
#define IDLE_SERVICE                                                           \
protected:                                                                     \
  void name(std::ostream& os) const override {                                 \
    static constexpr auto                                                      \
        type = type_name<unrefcv_t<std::remove_pointer_t<decltype(this)>>>();  \
    os << ::idle::StringView(type);                                            \
  }

/// Automatically assigns a type to an idle::Interface
///
/// The required method Interface::id(), Interface::type() and
/// Interface::classof() can be specified manually if this macro isn't used.
#define IDLE_INTERFACE                                                         \
public:                                                                        \
  static ::idle::Interface::Id id() noexcept {                                 \
    static constexpr ::idle::StringView name = IDLE_CURRENT_SOURCE_LOCATION()  \
                                                   .namespace_class_name();    \
    static ::idle::Interface::Id const this_id{name};                          \
    return this_id;                                                            \
  }                                                                            \
  ::idle::Interface::Id type() const noexcept override {                       \
    return id();                                                               \
  }                                                                            \
  static bool classof(::idle::Interface const& inter) noexcept {               \
    return inter.type() == id();                                               \
  }

#endif // IDLE_CORE_SUPPORT_HPP_INCLUDED
