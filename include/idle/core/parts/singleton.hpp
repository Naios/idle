
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

#ifndef IDLE_CORE_PARTS_SINGLETON_HPP_INCLUDED
#define IDLE_CORE_PARTS_SINGLETON_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>

namespace idle {
/// The Singleton makes it possible to specify an Interface where only
/// one instance is allowed to be running at the same time.
///
/// This works through a mutual system exclusion such that the owning service
/// can only be started when the interface is the one with the highest priority
/// and no other service with the same specified Interface is currently running.
class IDLE_API(idle) Singleton final : public Import, public Subscriber {
public:
  explicit Singleton(Interface& excluded) noexcept;

  Service& owner() noexcept override;
  Service const& owner() const noexcept override;

protected:
  void onPartInit() noexcept override;
  void onPartDestroy() noexcept override;
  void onSubscribedCreated(Interface& subscribed) override;
  void onSubscribedDestroy(Interface& subscribed) override;
  void onSubscribedLock(Interface& subscribed) noexcept override;
  void onSubscribedUnlocked(Interface& subscribed) noexcept override;

private:
  Interface& excluded_;
};
} // namespace idle

#endif // IDLE_CORE_PARTS_SINGLETON_HPP_INCLUDED
