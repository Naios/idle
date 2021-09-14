
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

#ifndef IDLE_CORE_PARTS_DYN_DEPENDENCIES_HPP_INCLUDED
#define IDLE_CORE_PARTS_DYN_DEPENDENCIES_HPP_INCLUDED

#include <utility>
#include <idle/core/api.hpp>
#include <idle/core/casting.hpp>
#include <idle/core/context.hpp>
#include <idle/core/detail/unordered_map.hpp>
#include <idle/core/parts/dependency_hooks.hpp>
#include <idle/core/parts/dependency_types.hpp>
#include <idle/core/parts/storage/void_storage.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/bitset.hpp>
#include <idle/core/util/intrusive_list.hpp>
#include <idle/core/util/upcastable.hpp>

namespace idle {
class IDLE_API(idle) DynDependenciesBase : public Import, protected Subscriber {
  class impl;
  friend class impl;

  enum class storage_state_t : std::uint8_t { erased, inserted, unknown };

  class IDLE_API(idle) UsageImpl final : public Usage,
                                         public Upcastable<UsageImpl> {
    friend class impl;

  public:
    UsageImpl(DynDependenciesBase& owner, Interface& used,
              BitSet<DependenciesFlags> flags) noexcept
      : use_reply_(UseReply::UseLater)
      , storage_state_(storage_state_t::erased)
      , is_weak_(flags.contains(DependenciesFlags::Weak))
      , owner_(owner)
      , used_(staticOf(used)) {}

    Interface& used() noexcept override {
      return *used_;
    }
    Interface const& used() const noexcept override {
      return *used_;
    }

    Import& user() noexcept override {
      return owner_;
    }
    Import const& user() const noexcept override {
      return owner_;
    }

  protected:
    void onUsedDestroy(Export& exp) override;
    void onUsedStarted(Export& exp) override;
    ReleaseReply onReleaseRequest(Export& exp) noexcept override;

  private:
    UseReply use_reply_ : 4;
    storage_state_t storage_state_ : 3;
    bool is_weak_ : 1;
    DynDependenciesBase& owner_;
    Ref<Interface> used_;
  };

public:
  explicit DynDependenciesBase(Service& owner);
  ~DynDependenciesBase() override;

  DynDependenciesBase(DynDependenciesBase&&) = delete;
  DynDependenciesBase(DynDependenciesBase const&) = delete;
  DynDependenciesBase& operator=(DynDependenciesBase&&) = delete;
  DynDependenciesBase& operator=(DynDependenciesBase const&) = delete;

  using Import::refCounter;
  using Import::weakRefCounter;

  Range<ImportList::iterator, ImportList::size_type> imports() noexcept final;
  Range<ImportList::const_iterator, ImportList::size_type>
  imports() const noexcept final;

  /// Returns the associated registry_entry
  Registry& registry() const noexcept {
    return *registry_entry_;
  }

protected:
  virtual Interface::Id type() const noexcept = 0;

  void onPartInit() noexcept override;
  void onPartDestroy() noexcept override;

  void onImportLock() noexcept override;
  void onImportUnlock() noexcept override;

  void onSubscribedCreated(Interface& inter) override;
  void onSubscribedDestroy(Interface& inter) override;
  void onSubscribedUnlocked(Interface& inter) override;

  void partName(std::ostream& os) const override;

  void doAcquireUsage(Interface& dep) noexcept;
  void doReleaseUsage(Interface& dep) noexcept;

  virtual void doInsert(Interface& dep, StorageUpdate type) noexcept = 0;
  virtual void doErase(Interface& dep, StorageUpdate type) noexcept = 0;

  virtual ReleaseReply onReleaseRequest(Interface& dep) noexcept = 0;
  virtual UseReply onUseOffer(Interface& dep) noexcept = 0;
  virtual BitSet<DependenciesFlags> onInspect(Interface& dep) noexcept = 0;

private:
  /// A reference to the registry entry of the dependency type
  Ref<Registry> registry_entry_;
  /// Mutates on the event loop reflects the services currently in use
  /// Contains usages of type usage_impl.
  ImportList used_;
  /// Holds the interface and their corresponding usages
  detail::unordered_node_map<Interface*, UsageImpl> map_;
};

namespace hooks {
/// Default release request handler that never releases
/// a dependency while running
struct OnReleaseRequestDefault {
  ReleaseReply operator()(Service const& current,
                          Interface const& dep) noexcept {
    (void)current;
    (void)dep;
    return ReleaseReply::Never;
  }
};

template <typename Owner, typename Dependency>
class OnReleaseRequestDyn : private OnReleaseRequestDefault {
  using value_type = ReleaseReply (*)(Owner&, Dependency&);

public:
  OnReleaseRequestDyn() noexcept = default;

  template <typename T, decltype(value_type(std::declval<T>()))* = nullptr>
  /* implicit */ OnReleaseRequestDyn(T&& value)
    : value_(static_cast<value_type>(std::forward<T>(value))) {}

  ReleaseReply operator()(Owner& owner, Dependency& dep) noexcept {
    if (value_) {
      return value_(owner, dep);
    } else {
      return OnReleaseRequestDefault::operator()(owner, dep);
    }
  }

private:
  value_type value_{nullptr};
};

/// Default use offer handler that never uses the service when running already
struct OnUseOfferDefault {
  UseReply operator()(Service const& current, Interface const& dep) noexcept {
    (void)current;
    (void)dep;
    return UseReply::UseNever;
  }
};

template <typename Owner, typename Dependency>
class OnUseOfferDyn : private OnUseOfferDefault {
  using value_type = UseReply (*)(Owner&, Dependency&);

public:
  OnUseOfferDyn() noexcept = default;

  template <typename T, decltype(value_type(std::declval<T>()))* = nullptr>
  /* implicit */ OnUseOfferDyn(T&& value)
    : value_(static_cast<value_type>(std::forward<T>(value))) {}

  UseReply operator()(Owner& owner, Dependency& dep) noexcept {
    if (value_) {
      return value_(owner, dep);
    } else {
      return OnUseOfferDefault::operator()(owner, dep);
    }
  }

private:
  value_type value_{nullptr};
};
} // namespace hooks

/// Implements a multi dynamic dependency class whose dependencies are
/// changeable on request at runtime.
template <typename Dependency, typename Owner = Service,
          typename Storage = VoidStorage<Dependency>,
          typename OnReleaseReq = hooks::OnReleaseRequestDyn<Owner, Dependency>,
          typename OnUseOffer = hooks::OnUseOfferDyn<Owner, Dependency>,
          typename OnInspect = hooks::OnInspectDyn<Owner, Dependency>>
class DynDependencies : public DynDependenciesBase,
                        private OnReleaseReq,
                        private OnUseOffer,
                        private OnInspect,
                        public Storage {
public:
  explicit DynDependencies(Owner& owner, OnReleaseReq on_release_req = {},
                           OnUseOffer onUseOffer = {}, OnInspect on_filter = {})
    : DynDependenciesBase(owner)
    , OnReleaseReq(std::move(on_release_req))
    , OnUseOffer(std::move(onUseOffer))
    , OnInspect(std::move(on_filter))
    , owner_(owner) {}
  explicit DynDependencies(Owner& owner, OnReleaseReq on_release_req,
                           OnUseOffer onUseOffer, OnInspect on_stat,
                           Storage storage)
    : DynDependenciesBase(owner)
    , OnReleaseReq(std::move(on_release_req))
    , OnUseOffer(std::move(onUseOffer))
    , OnInspect(std::move(on_stat))
    , Storage(std::move(storage))
    , owner_(owner) {}

  DynDependencies(DynDependencies&&) = delete;
  DynDependencies(DynDependencies const&) = delete;
  DynDependencies& operator=(DynDependencies&&) = delete;
  DynDependencies& operator=(DynDependencies const&) = delete;

  Owner& owner() noexcept override {
    return owner_;
  }
  Owner const& owner() const noexcept override {
    return owner_;
  }

  /// Acquires an usage to the given dependency
  void acquireUsage(Dependency& dep) noexcept {
    DynDependenciesBase::doAcquireUsage(dep);
  }
  /// Releases an usage to the given dependency
  void releaseUsage(Dependency& dep) noexcept {
    DynDependenciesBase::doReleaseUsage(dep);
  }

  /// Inserts the given dependency into the user storage
  void insert(Dependency& dep) noexcept {
    // User changes are only permitted in a locked state
    IDLE_ASSERT(owner().root().is_on_event_loop() || isLocked());
    IDLE_ASSERT(owner().root().is_on_event_loop() ||
                dep.owner().state().isRunning());
    Storage::onInsert(dep, StorageUpdate::Manual);
  }
  /// Erases the given dependency from the user storage
  void erase(Dependency& dep) noexcept {
    // User changes are only permitted in a locked state
    IDLE_ASSERT(owner().root().is_on_event_loop() || isLocked());
    Storage::onErase(dep, StorageUpdate::Manual);
  }

protected:
  Interface::Id type() const noexcept override {
    return Dependency::id();
  }

  void onImportLock() noexcept override {
    IDLE_ASSERT(owner().root().is_on_event_loop());
    DynDependenciesBase::onImportLock();
    Storage::onLock(*this);
  }

  void onImportUnlock() noexcept override {
    IDLE_ASSERT(owner().root().is_on_event_loop());
    Storage::onUnlock(*this);
    DynDependenciesBase::onImportUnlock();
  }

  void doInsert(Interface& dep, StorageUpdate type) noexcept override {
    IDLE_ASSERT(type != StorageUpdate::Manual);
    Storage::onInsert(cast<Dependency>(dep), type);
  }

  void doErase(Interface& dep, StorageUpdate type) noexcept override {
    IDLE_ASSERT(type != StorageUpdate::Manual);
    Storage::onErase(cast<Dependency>(dep), type);
  }

  ReleaseReply onReleaseRequest(Interface& dep) noexcept override {
    return OnReleaseReq::operator()(owner_, cast<Dependency>(dep));
  }
  UseReply onUseOffer(Interface& dep) noexcept override {
    return OnUseOffer::operator()(owner_, cast<Dependency>(dep));
  }
  BitSet<DependenciesFlags> onInspect(Interface& dep) noexcept override {
    return OnInspect::operator()(owner_, cast<Dependency>(dep));
  }

private:
  Owner& owner_;
};
} // namespace idle

#endif // IDLE_CORE_PARTS_DYN_DEPENDENCIES_HPP_INCLUDED
