
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

#include <shared_mutex>
#include <utility>
#include <idle/core/context.hpp>
#include <idle/core/parts/dependency_list.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/functional.hpp>
#include <idle/interface/activity.hpp>

namespace idle {
class ActivityImpl final : public Activity {
public:
  ActivityImpl(std::string&& name_, float progress_)
    : name(std::move(name_))
    , progress(progress_) {}

  std::string name;
  std::atomic<float> progress{0};
};

std::string const& Activity::name() const noexcept {
  auto const& impl = static_cast<ActivityImpl const&>(*this);
  return impl.name;
}

float Activity::progress() const noexcept {
  auto const& impl = static_cast<ActivityImpl const&>(*this);
  return impl.progress.load(std::memory_order_relaxed);
}

void Activity::update(float progress) noexcept {
  IDLE_ASSERT(progress <= 1.f);
  IDLE_ASSERT(progress >= 0.f);

  auto& impl = static_cast<ActivityImpl&>(*this);
  impl.progress.store(progress, std::memory_order_relaxed);
}

void ActivityListener::onActivityAdd(Activity const& activity) noexcept {
  (void)activity;

  IDLE_ASSERT(owner().root().is_on_event_loop());
}

void ActivityListener::onActivityDel(Activity const& activity) noexcept {
  (void)activity;

  IDLE_ASSERT(owner().root().is_on_event_loop());
}

void ActivityListener::onActivityUpdate(Activity const& activity) noexcept {
  (void)activity;

  IDLE_ASSERT(owner().root().is_on_event_loop());
}

class DefaultActivities final : public Implements<Activities> {
  friend class ActivityHandle;

  // using Mutex = std::shared_mutex;

public:
  using Implements<Activities>::Implements;

  ActivitySet const& activitiesImpl() const noexcept {
    IDLE_ASSERT(root().is_on_event_loop());

    // std::shared_lock<Mutex> lock(mutex_);
    // Activities current = activities_;
    // return current;
    return activities_;
  }

  ActivityHandle addImpl(std::string&& name) noexcept {
    IDLE_ASSERT(root().is_on_event_loop());

    auto activity = make_ref<ActivityImpl>(std::move(name), 0.f);

    {
      // std::unique_lock<Mutex> lock(mutex_);
      activities_.insert(activity);
    }

    for (ActivityListener& listener : listeners_) {
      listener.onActivityAdd(*activity);
    }

    activities_count_.fetch_add(1, std::memory_order_relaxed);

    return ActivityHandle(std::move(activity),
                          handleOf(static_cast<Activities&>(*this)));
  }

  void delImpl(Ref<Activity> const& activity) noexcept {
    IDLE_ASSERT(root().is_on_event_loop());

    {
      // std::unique_lock<Mutex> lock(mutex_);
      activities_.erase(activity);
    }

    activities_count_.fetch_sub(1, std::memory_order_relaxed);

    for (ActivityListener& listener : listeners_) {
      listener.onActivityDel(*activity);
    }
  }

  std::size_t countUnsafeImpl() const noexcept {
    return activities_count_.load(std::memory_order_relaxed);
  }

private:
  ActivitySet activities_;
  // mutable Mutex mutex_;

  std::atomic<std::size_t> activities_count_{0};

  DynDependencyList<ActivityListener, DefaultActivities> listeners_{
      *this,                                         //
      IDLE_STATIC_RETURN(ReleaseReply::Now),         //
      IDLE_STATIC_RETURN(UseReply::UseNowAndInsert), //
      [](auto&, auto&) {
        return BitSet<DependenciesFlags>{DependenciesFlags::Weak};
      }};

  IDLE_SERVICE
};

Activities::ActivitySet const& Activities::activities() const noexcept {
  return static_cast<DefaultActivities const*>(this)->activitiesImpl();
}

std::size_t Activities::countUnsafe() const noexcept {
  return static_cast<DefaultActivities const*>(this)->countUnsafeImpl();
}

ActivityHandle Activities::add(std::string name) {
  return static_cast<DefaultActivities*>(this)->addImpl(std::move(name));
}

Ref<Activities> Activities::create(Inheritance parent) {
  return spawn<DefaultActivities>(std::move(parent));
}

void ActivityHandle::reset() noexcept {
  if (activity_) {
    if (auto dist = distributor_.lock()) {
      Context& root = dist->owner().root();

      root.event_loop().dispatch([distributor = std::move(distributor_),
                                  activity = std::move(activity_)] {
        if (auto dist = distributor.lock()) {
          static_cast<DefaultActivities&>(*dist).delImpl(activity);
        }
      });
    }

    activity_.reset();
  }
}

ActivityHandle::~ActivityHandle() {
  reset();
}
} // namespace idle
