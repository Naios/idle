
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

#ifndef IDLE_CORE_PARTS_DEPENDENCY_HOOKS_HPP_INCLUDED
#define IDLE_CORE_PARTS_DEPENDENCY_HOOKS_HPP_INCLUDED

#include <cstdint>
#include <idle/core/api.hpp>
#include <idle/core/parts/dependency_types.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/bitset.hpp>

namespace idle {
enum class DependenciesFlags : std::uint8_t {
  Ignore, ///< By default never add the dependency automatically
  Weak    ///< Acquire a weak Usage to the dependency instead of a strong one
};

namespace hooks {
/// Default filter for dependencies which never filters dependencies out
struct OnInspectDefault {
  BitSet<DependenciesFlags> operator()(Service& current,
                                       Interface& dep) const noexcept {
    (void)current;
    (void)dep;
    return {};
  }
};

template <typename Owner, typename Dependency>
class OnInspectDyn : private OnInspectDefault {
  using value_type = BitSet<DependenciesFlags> (*)(Owner&, Dependency&);

public:
  OnInspectDyn() noexcept = default;

  template <typename T, decltype(value_type(std::declval<T>()))* = nullptr>
  /* implicit */ OnInspectDyn(T&& value)
    : value_(static_cast<value_type>(std::forward<T>(value))) {}

  BitSet<DependenciesFlags> operator()(Owner& owner,
                                       Dependency& dep) const noexcept {
    if (value_) {
      return value_(owner, dep);
    } else {
      return OnInspectDefault::operator()(owner, dep);
    }
  }

private:
  value_type value_{nullptr};
};
} // namespace hooks
} // namespace idle

#endif // IDLE_CORE_PARTS_DEPENDENCY_HOOKS_HPP_INCLUDED
