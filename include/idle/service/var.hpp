
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

#ifndef IDLE_SERVICE_VAR_HPP_INCLUDED
#define IDLE_SERVICE_VAR_HPP_INCLUDED

#include <memory>
#include <string>
#include <type_traits>
#include <idle/core/api.hpp>
#include <idle/core/context.hpp>
#include <idle/core/detail/chaining.hpp>
#include <idle/core/interface.hpp>
#include <idle/core/parts/dyn_dependency.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/utility.hpp>
#include <idle/service/art/equals.hpp>
#include <idle/service/art/reflection.hpp>
#include <idle/service/detail/callable_traits.hpp>

namespace idle {
class VarBase;
class Properties;

class IDLE_API(idle) VarBase : protected DynDependencyBase {
public:
  explicit VarBase(Service& owner, std::string key);

  std::string const& key() const noexcept {
    return key_;
  }

protected:
  void partName(std::ostream& os) const override;

  void onImportLock() noexcept override;
  void onImportUnlock() noexcept override;

  Properties& raw() noexcept;
  Properties const& raw() const noexcept;

  Interface::Id type() const noexcept override;

  void save(ConstReflectionPtr reflected) noexcept;

private:
  std::string const key_;
};

struct MoveApplier {
  template <typename T>
  void operator()(T& to, T&& from) {
    to = std::move(from);
  }
};

template <typename T>
struct DefaultConstruct {
  T operator()() const {
    return {};
  }
};

template <typename T>
class VarUpdate {
  using const_trait = detail::chain_trait<T const>;

public:
  explicit VarUpdate(SwapAction<Interface> action, T&& src, T& dest)
    : action_(std::move(action))
    , src_(std::move(src))
    , dest_(std::addressof(dest)) {}

  template <typename Applier = MoveApplier>
  bool apply(Applier&& applier = {}) noexcept {
    IDLE_ASSERT(dest_);

    auto const locked = action_.handle().lock();
    if (!locked || !action_.apply()) {
      return false;
    }

    std::forward<Applier>(applier)(*dest_, std::move(src_));
    dest_ = nullptr;
    return true;
  }

  T const& operator*() const noexcept {
    return src_;
  }

  typename const_trait::type operator->() const noexcept {
    return const_trait::do_chain(static_cast<T const&>(src_));
  }

private:
  SwapAction<Interface> action_;
  T src_;
  T* dest_;
};

namespace hooks {
template <typename Owner, typename T>
class OnVarUpdateDyn {
  using value_type = bool (*)(Owner&, VarUpdate<T>&& update);

public:
  OnVarUpdateDyn() noexcept = default;

  template <typename V, decltype(value_type(std::declval<V>()))* = nullptr>
  /* implicit */ OnVarUpdateDyn(V&& value)
    : value_(static_cast<value_type>(std::forward<V>(value))) {}

  bool operator()(Owner& owner, VarUpdate<T>&& update) noexcept {
    IDLE_ASSERT(value_);
    return value_(owner, std::move(update));
  }

  explicit operator bool() const noexcept {
    return !!value_;
  }

private:
  value_type value_{nullptr};
};
} // namespace hooks

// TODO Var vs DynVar?
template <typename T, typename Owner = Service,
          typename OnVarUpdate = hooks::OnVarUpdateDyn<Owner, T>,
          typename DefaultFactory = DefaultConstruct<T>>
class Var final : public VarBase, private OnVarUpdate, private DefaultFactory {
  using traits = detail::def_chain_traits<T>;

  static_assert(std::is_copy_constructible<T>::value,
                "A config type must be default constructible!");

public:
  using Update = VarUpdate<T>;

  explicit Var(Owner& owner, std::string key, DefaultFactory factory = {},
               OnVarUpdate on_var_update = {})
    : VarBase(owner, std::move(key))
    , OnVarUpdate(std::move(on_var_update))
    , DefaultFactory(std::move(factory))
    , owner_(owner)
    , event_space_current_(createDefaultValue())
    , user_space_current_(event_space_current_) {}

  Owner& owner() noexcept override {
    return owner_;
  }
  Owner const& owner() const noexcept override {
    return owner_;
  }

  T const& operator*() const noexcept {
    IDLE_ASSERT(isLocked());
    return user_space_current_;
  }

  typename traits::const_trait::type operator->() const noexcept {
    return traits::const_trait::do_chain(as_const(operator*()));
  }

  T const* get() const noexcept {
    return std::addressof(operator*());
  }

  void set(T replacement) {
    if (!art::equals(replacement, user_space_current_)) {
      Context& root = owner().root();

      if (root.is_on_event_loop()) {
        user_space_current_ = std::move(replacement);
        event_space_current_ = user_space_current_;

        save(user_space_current_);
      } else {
        user_space_current_ = std::move(replacement);

        // If we cannot dispatch the change directly,
        // create a copy of the replacement.
        root.event_loop().post(
            weakly(handleOf(*this), [rep = user_space_current_](Ref<Var>&& me) {
              me.get()->save(rep);
              me.get()->event_space_current_ = std::move(rep);
            }));
      }
    }
  }

  Var& operator=(T replacement) {
    set(std::move(replacement));
    return *this;
  }

protected:
  bool canSwap(Nullable<Interface> from,
               Nullable<Interface> to) noexcept override {
    IDLE_ASSERT(from);

    if (!to) {
      return false;
    }

    // Swap to the latest properties generation if the content matches
    T src(createDefaultValue());
    cast<Properties>(*from).get(src, key());
    T target(createDefaultValue());
    cast<Properties>(*to).get(target, key());
    if (art::equals(src, target)) {
      return true;
    }

    return hasHook();
  }

  void onSwap(SwapAction<Interface>&& action) noexcept override {
    IDLE_ASSERT(isLocked());

    Properties const& to = cast<Properties>(*action.to());

    T updated(createDefaultValue());
    to.get(updated, key());

    // We have to check against the real object to take the
    // default object state into account.
    if (art::equals(updated, event_space_current_)) {
      // Just swap in-place if the new value matches the old one
      action.apply();
      return;
    }

    IDLE_ASSERT(hasHook());

    event_space_current_ = updated;

    OnVarUpdate::operator()(owner_,
                            Update(std::move(action), std::move(updated),
                                   user_space_current_));
  }

  void onImportLock() noexcept override {
    VarBase::onImportLock();

    // Initialize the value from the source
    raw().get(event_space_current_, key());
    user_space_current_ = event_space_current_;

    // Save the properties to the source to store comments
    // and eventually added fields with default values.
    save(event_space_current_);
  }

  void onImportUnlock() noexcept override {
    VarBase::onImportUnlock();

    event_space_current_ = createDefaultValue();
    user_space_current_ = event_space_current_;
  }

private:
  constexpr bool hasHook() const noexcept {
    return detail::callable_present_trait<OnVarUpdate>::is_present(*this);
  }

  T createDefaultValue() const {
    return DefaultFactory::operator()();
  }

  Owner& owner_;
  T event_space_current_;
  T user_space_current_;
};
} // namespace idle

#endif // IDLE_SERVICE_VAR_HPP_INCLUDED
