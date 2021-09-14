
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

#ifndef IDLE_INTERFACE_ACTIVITY_HPP_INCLUDED
#define IDLE_INTERFACE_ACTIVITY_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/use.hpp>
#include <idle/core/util/flat_set.hpp>

namespace idle {
class Activities;

class IDLE_API(idle) Activity : public ReferenceCounted {
  friend class ActivityImpl;

public:
  std::string const& name() const noexcept;

  float progress() const noexcept;
  void update(float progress) noexcept;
};

class IDLE_API(idle) ActivityListener : public Interface {
  friend class DefaultActivities;

public:
  using Interface::Interface;

protected:
  virtual void onActivityAdd(Activity const& activity) noexcept;
  virtual void onActivityDel(Activity const& activity) noexcept;
  virtual void onActivityUpdate(Activity const& activity) noexcept;

  IDLE_INTERFACE
};

class IDLE_API(idle) ActivityHandle {
  friend class DefaultActivities;

  ActivityHandle(Ref<Activity> activity, Handle<Activities> distributor)
    : activity_(std::move(activity))
    , distributor_(std::move(distributor)) {}

public:
  ~ActivityHandle();

  ActivityHandle(ActivityHandle const&) = delete;
  ActivityHandle(ActivityHandle&&) = default;
  ActivityHandle& operator=(ActivityHandle const&) = delete;
  ActivityHandle& operator=(ActivityHandle&&) = default;

  Activity* operator->() const noexcept {
    return activity_.get();
  }
  Activity& operator*() const noexcept {
    return *activity_;
  }

  void reset() noexcept;

private:
  Ref<Activity> activity_;
  Handle<Activities> distributor_;
};

class IDLE_API(idle) Activities : public Interface {
  friend class DefaultActivities;
  using Interface::Interface;

public:
  struct ActivitiesPredicate {
    bool operator()(Ref<Activity const> const& left,
                    Ref<Activity const> const& right) const noexcept {
      return left.get() < right.get();
    }
  };

  using ActivitySet = FlatSet<Ref<Activity const>, ActivitiesPredicate>;

  /// Returns all registered activities
  ///
  /// \attention avoid to call this method recurrently, listen on changes
  /// through a registered ActivityListener instead!
  ActivitySet const& activities() const noexcept;

  /// Returns an approximate count of the current activities
  std::size_t countUnsafe() const noexcept;

  /// Adds an activity
  ActivityHandle add(std::string name);

  static Ref<Activities> create(Inheritance parent);

  IDLE_INTERFACE
};
} // namespace idle

#endif // IDLE_INTERFACE_ACTIVITY_HPP_INCLUDED
