
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

#ifndef IDLE_CORE_USE_HPP_INCLUDED
#define IDLE_CORE_USE_HPP_INCLUDED

#include <memory>
#include <utility>
#include <idle/core/async.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/assert.hpp>

namespace idle {
namespace detail {
template <typename T, std::enable_if_t<is_service<T>::value>* = nullptr>
Service const& disambiguate(T const& actor) {
  return static_cast<Service const&>(actor);
}
template <typename T, std::enable_if_t<!is_service<T>::value>* = nullptr>
Service const& disambiguate(T const& actor) {
  return actor.owner();
}
} // namespace detail

/// A direct use to a Service ensures that a Service is running while the use is
/// active. Acquiring an active Use to a Service is required if an asynchronous
/// task has to ensure that its issuing Service is still valid on execution.
///
/// A Use is the a strong acquisition of a Handle, comparable to a
/// std::shared_ptr which is a strong acquisition of a std::weak_ptr.
template <typename T>
class Use {
  using chain_trait = detail::chain_trait<T>;

  explicit Use(T& current)
    : obj_(std::addressof(current)) {}

public:
  Use() noexcept = default;

  Use(Use const& other) noexcept
    : obj_(other.obj_) {
    if (obj_) {
      detail::inc_use(detail::disambiguate(other));
    }
  }
  Use(Use&& other) noexcept
    : obj_(std::exchange(other.obj_, {})) {}
  Use& operator=(Use const& other) noexcept {
    if (other.obj_) {
      detail::inc_use(detail::disambiguate(*other.obj_));
    }
    if (obj_) {
      detail::dec_use(detail::disambiguate(*obj_));
    }
    obj_ = other.obj_;
    return *this;
  }
  Use& operator=(Use&& other) noexcept {
    if (obj_) {
      detail::dec_use(detail::disambiguate(*obj_));
    }
    obj_ = std::exchange(other.obj_, {});
    return *this;
  }
  ~Use() noexcept {
    if (obj_) {
      detail::dec_use(detail::disambiguate(*obj_));
      obj_ = nullptr;
    }
  }

  constexpr T& operator*() const noexcept {
    IDLE_ASSERT(obj_);
    return *obj_;
  }

  constexpr T* get() const noexcept {
    IDLE_ASSERT(obj_);
    return obj_;
  }
  typename chain_trait::type operator->() const noexcept {
    return chain_trait::do_chain(*get());
  }
  constexpr explicit operator bool() const noexcept {
    return obj_;
  }

  static Use<T> tryUse(T& current) {
    if (detail::try_use(detail::disambiguate(current))) {
      return Use(current);
    } else {
      return {};
    }
  }

private:
  T* obj_{nullptr};
};

template <typename T>
Use<T> useOf(T& service) noexcept {
  IDLE_ASSERT(detail::is_on_event_loop(service));
  Use<T> current = Use<T>::tryUse(service);
  IDLE_ASSERT(current);
  return current;
}

template <typename T>
class Handle {
public:
  explicit Handle(T& obj, WeakRefCounter counter, Epoch epoch) noexcept
    : obj_(std::addressof(obj))
    , counter_(std::move(counter))
    , epoch_(epoch) {}
  Handle(Handle const& other) noexcept
    : obj_(other.obj_)
    , counter_(other.counter_)
    , epoch_(other.epoch_) {

    IDLE_ASSERT(obj_);
  }
  Handle(Handle&& other) noexcept
    : obj_(std::exchange(other.obj_, {}))
    , counter_(std::exchange(other.counter_, {}))
    , epoch_(other.epoch_) {

    IDLE_ASSERT(obj_);
  }
  Handle& operator=(Handle const& other) noexcept {
    IDLE_ASSERT(other.obj_);
    if (this == &other) {
      return *this;
    }

    obj_ = other.obj_;
    counter_ = other.counter_;
    epoch_ = other.epoch_;
    return *this;
  }
  Handle& operator=(Handle&& other) noexcept {
    IDLE_ASSERT(other.obj_);

    obj_ = std::exchange(other.obj_, {});
    counter_ = std::exchange(other.counter_, {});
    epoch_ = other.epoch_;
    return *this;
  }

  /// Returns a valid reference to the service if the service is still
  /// in the same epoch as on the creation of this handle.
  ///
  /// \attention Locking the reference outside of the event loop is potentially
  ///            unsafe if the service is required to stay in the same epoch
  ///		 while the reference is used. Perform your action on the
  ///		 event loop if you are requiring this.
  Ref<T> lockRef() const noexcept {
    IDLE_ASSERT(obj_ && "Attempted to lock an uninitialized or moved handle!");

    if (counter_) {
      if (RefCounter strong = counter_.lock()) {
        return Ref<T>(obj_, strong);
      }
    } else {
      return Ref<T>(obj_, {});
    }
    return {};
  }

  /// Returns a valid reference to the service if the service is still
  /// in the same epoch as on the creation of this handle.
  Use<T> lock() const noexcept {
    // Perform a 2-way lock to acquire an active
    // use that is guaranteed to be in the same epoch.
    if (auto const locked = lockRef()) {
      if (auto current = Use<T>::tryUse(*locked)) {
        if (current->epoch() == epoch_) {
          return current;
        }
      }
    }
    return {};
  }

  template <typename Target>
  Handle<Target> pin(Target& target) const& {
    return Handle<Target>(target, counter_, epoch_);
  }
  template <typename Target>
  Handle<Target> pin(Target& target) && {
    return Handle<Target>(target, std::move(counter_), std::move(epoch_));
  }

  Epoch epoch() const noexcept {
    return epoch_;
  }

private:
  T* obj_;
  WeakRefCounter counter_;
  Epoch epoch_;
};

template <typename Actor,
          typename Handle =
              Handle<std::remove_const_t<std::remove_reference_t<Actor>>>>
Handle handleOf(Actor& actor) noexcept {
  auto epoch = actor.epoch();
  return Handle(actor, actor.weakRefCounter(), std::move(epoch));
}

template <typename Actor, typename Callable>
auto wrap(Actor& actor, Callable&& callable) {
  return weakly(handleOf(actor), std::forward<Callable>(callable));
}

template <typename Actor, typename Callable>
auto asyncWrap(Actor& actor, Callable&& callable) {
  return async(wrap(actor, std::forward<Callable>(callable)));
}

template <typename Actor, typename Callable, typename Executor>
auto asyncWrap(Actor& actor, Callable&& callable, Executor&& executor) {
  return async(wrap(actor, std::forward<Callable>(callable),
                    std::forward<Executor>(executor)));
}
} // namespace idle

#endif // IDLE_CORE_USE_HPP_INCLUDED
