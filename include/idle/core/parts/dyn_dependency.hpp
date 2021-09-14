
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

#ifndef IDLE_CORE_PARTS_DYN_DEPENDENCY_HPP_INCLUDED
#define IDLE_CORE_PARTS_DYN_DEPENDENCY_HPP_INCLUDED

#include <atomic>
#include <idle/core/api.hpp>
#include <idle/core/detail/registry_traits.hpp>
#include <idle/core/parts/dependency_hooks.hpp>
#include <idle/core/parts/dependency_types.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/use.hpp>
#include <idle/core/util/intrusive_list.hpp>

namespace idle {
namespace detail {
template <typename T>
class DependencyUsage : public Usage {
public:
  DependencyUsage() = default;

  Export& used() noexcept override {
    return *ref_;
  }
  Export const& used() const noexcept override {
    return *ref_;
  }

  Ref<Interface>& ptr() noexcept {
    return ref_;
  }
  Ref<Interface> const& ptr() const noexcept {
    return ref_;
  }

private:
  Ref<Interface> ref_;
};

struct ActiveTag {};
struct PreferredTag {};
} // namespace detail

template <typename = Interface>
class SwapAction;

class IDLE_API(idle) DynDependencyBase
  : public Import,
    protected Subscriber,
    protected detail::DependencyUsage<detail::ActiveTag>,
    protected detail::DependencyUsage<detail::PreferredTag> {

  template <typename>
  friend class SwapAction;
  friend class DynDependencyBaseImpl;

public:
  explicit DynDependencyBase(Service& owner);

  using Import::refCounter;
  using Import::weakRefCounter;

  Range<ImportList::iterator, ImportList::size_type> imports() noexcept final;
  Range<ImportList::const_iterator, ImportList::size_type>
  imports() const noexcept final;

  bool operator==(DynDependencyBase const& right) const noexcept {
    return &raw() == &right.raw();
  }

  bool operator!=(DynDependencyBase const& right) const noexcept {
    return &raw() != &right.raw();
  }

protected:
  virtual Interface::Id type() const noexcept = 0;

  void onPartInit() noexcept override;
  void onPartDestroy() noexcept override;

  void onImportLock() noexcept override;
  void onImportUnlock() noexcept override;

  void onSubscribedCreated(Interface& inter) override;
  void onUsedStarted(Export& exp) override;
  void onUsedDestroy(Export& exp) override;

  Import& user() noexcept override {
    return *this;
  }
  Import const& user() const noexcept override {
    return *this;
  }

  Interface& raw() noexcept {
    Interface* current = userspace_ptr_.load(std::memory_order_relaxed);
    IDLE_ASSERT(current);
    return *current;
  }
  Interface const& raw() const noexcept {
    Interface const* current = userspace_ptr_.load(std::memory_order_relaxed);
    IDLE_ASSERT(current);
    return *current;
  }

  ReleaseReply onReleaseRequest(Export& exp) noexcept final;

  virtual BitSet<DependenciesFlags> onInspect(Interface& dep) noexcept;
  virtual bool canSwap(Nullable<Interface> from,
                       Nullable<Interface> to) noexcept;
  virtual void onSwap(SwapAction<Interface>&& action) noexcept;

  virtual bool hasDefault() const noexcept;
  virtual Ref<Interface> createDefault();

  void partName(std::ostream& os) const override;

private:
  DependencyUsage<detail::ActiveTag>& active() noexcept {
    return *this;
  }
  DependencyUsage<detail::ActiveTag> const& active() const noexcept {
    return *this;
  }
  DependencyUsage<detail::PreferredTag>& pref() noexcept {
    return *this;
  }
  DependencyUsage<detail::PreferredTag> const& pref() const noexcept {
    return *this;
  }

  void offerSwap() noexcept;
  static bool apply(Handle<DynDependencyBase>&& handle, Interface* from,
                    Interface* to);
  void store(Interface* preferred) noexcept;
  void finalize() noexcept;

  void setDefault() noexcept;

  Ref<Registry> registry_entry_;
  std::atomic<Interface*> userspace_ptr_;
  bool pending_;
};

namespace detail {
template <typename To, typename From>
auto possible_null_cast(From* source) noexcept -> decltype(cast<To>(source)) {
  if (source) {
    return cast<To>(source);
  } else {
    return nullptr;
  }
}
} // namespace detail

template <typename T>
class SwapAction {
  friend class DynDependencyBase;

  explicit SwapAction(Handle<DynDependencyBase> handle, T* from, T* to)
    : from_(from)
    , to_(to)
    , handle_(std::move(handle)) {}

public:
  template <typename O, std::enable_if_t<!std::is_same<T, O>::value>* = nullptr>
  explicit SwapAction(SwapAction<O>&& other)
    : from_(std::exchange(detail::possible_null_cast(other.from_), nullptr))
    , to_(std::exchange(detail::possible_null_cast(other.to), nullptr))
    , handle_(std::move(other.handle_)) {}

  /// Updates the userspace pointers of the DynDependency to point to the latest
  /// available preferred Interface.
  ///
  /// \returns false if the userspace pointer could not be swapped because
  ///          the action was outdated due to a Service stop,
  ///          returns true otherwise.
  bool apply() noexcept {
    return DynDependencyBase::apply(std::move(handle_),
                                    std::exchange(from_, nullptr),
                                    std::exchange(to_, nullptr));
  }

  Nullable<T> from() noexcept {
    return from_;
  }

  Nullable<T> to() noexcept {
    return to_;
  }

  Handle<DynDependencyBase> const& handle() const noexcept {
    return handle_;
  }

private:
  T* from_;
  T* to_;
  Handle<DynDependencyBase> handle_;
};

namespace hooks {
/// Default filter for dependencies which never filters dependencies out
template <typename Dependency = Interface>
struct OnSwapDefault {
  void operator()(Service& current,
                  SwapAction<Dependency>&& action) const noexcept {
    (void)current;
    (void)action;
  }
};

template <typename Owner, typename Dependency = Interface>
class OnSwapDyn : private OnSwapDefault<Dependency> {
  using value_type = void (*)(Owner&, SwapAction<Dependency>&& action);

public:
  OnSwapDyn() noexcept = default;

  template <typename T, decltype(value_type(std::declval<T>()))* = nullptr>
  /* implicit */ OnSwapDyn(T&& value)
    : value_(static_cast<value_type>(std::forward<T>(value))) {}

  void operator()(Owner& owner,
                  SwapAction<Dependency>&& action) const noexcept {
    if (value_) {
      return value_(owner, std::move(action));
    } else {
      return OnSwapDefault<Dependency>::operator()(owner, std::move(action));
    }
  }

private:
  value_type value_{nullptr};
};
} // namespace hooks

template <typename Dependency, typename Owner = Service,
          typename OnSwap = hooks::OnSwapDyn<Owner, Dependency>,
          typename OnInspect = hooks::OnInspectDyn<Owner, Dependency>>
class DynDependency final : public DynDependencyBase {
  using chain_trait = detail::chain_trait<Dependency>;
  using const_chain_trait = detail::chain_trait<Dependency const>;
  static_assert(
      chain_trait::value == const_chain_trait::value,
      "The dependency has a mismatching operator-> behaviour because of "
      "const correctness. Consider implementing an operator-> for both cases "
      "or qualify operator-> as const!");

public:
  explicit DynDependency(Owner& owner, OnSwap on_swap = {},
                         OnInspect on_filter = {})
    : DynDependencyBase(owner)
    , OnSwap(std::move(on_swap))
    , OnInspect(std::move(on_filter))
    , owner_(owner) {}

  typename chain_trait::type operator->() noexcept {
    return chain_trait::do_chain(cast<Dependency>(raw()));
  }

  typename const_chain_trait::type operator->() const noexcept {
    return const_chain_trait::do_chain(cast<Dependency>(raw()));
  }

  Dependency& operator*() noexcept {
    return cast<Dependency>(raw());
  }

  Dependency const& operator*() const noexcept {
    return cast<Dependency>(raw());
  }

  Owner& owner() noexcept override {
    return owner_;
  }

  Owner const& owner() const noexcept override {
    return owner_;
  }

protected:
  Interface::Id type() const noexcept override {
    return Dependency::id();
  }

  bool hasDefault() const noexcept override {
    return detail::has_create<Dependency>::value;
  }

  Ref<Interface> createDefault() override {
    return detail::do_create<Dependency>(
        Inheritance::weak(owner().root().anchor(),
                          Inheritance::Relation::anchor));
  }

  void onSwap(SwapAction<Interface>&& action) noexcept override {
    return OnSwap::operator()(owner_, std::move(action));
  }

  BitSet<DependenciesFlags> onInspect(Interface& dep) noexcept override {
    return OnInspect::operator()(owner_, cast<Dependency>(dep));
  }

private:
  Owner& owner_;
};
} // namespace idle

#endif // IDLE_CORE_PARTS_DYN_DEPENDENCY_HPP_INCLUDED
