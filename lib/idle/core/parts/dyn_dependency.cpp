
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

#include <utility>
#include <idle/core/context.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/parts/dyn_dependency.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/printable.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
static bool IsPrioritizedOverCurrent(Interface const& current,
                                     Interface const& other) noexcept {
  return other > current;
}

static Ref<Interface> reference_of(Interface& inter) {
  if (inter.owner().state().isDefaultCreated()) {
    // We have to know whether the interface is exported by a default
    // service for this interface, if so, make the reference strong.
    return refOf(inter);
  } else {
    // Never acquire a strong reference to preferred services when those
    // are not requested explicitly.
    return staticOf(inter);
  }
}

class DynDependencyBaseImpl {
public:
  static Nullable<Interface>
  reselect_preferred(DynDependencyBase const& me,
                     Interface const& exclude) noexcept {

    IDLE_ASSERT(me.active().ptr());

    for (Interface& available : me.registry_entry_->interfaces()) {
      if (available == exclude) {
        continue;
      }

      if (available == *me.active().ptr()) {
        break;
      }

      return available;
    }

    return {};
  }

  static void reconnect_preferred(DynDependencyBase& me,
                                  Nullable<Interface> replacement) noexcept {
    IDLE_ASSERT(me.active().ptr());
    IDLE_ASSERT(me.pref().ptr());

    me.pref().disconnect();

    if (replacement) {
      me.pref().ptr() = reference_of(*replacement);
      me.pref().connect();
    } else {
      me.pref().ptr().reset();
      ImportList::disconnect(me.pref());
    }
  }

  static Nullable<Interface> get_next_preferred(DynDependencyBase& me,
                                                Interface& current) noexcept {
    if (current == me.active().used()) {
      if (me.pref().ptr()) {
        return *me.pref().ptr();
      } else {
        return {};
      }
    } else {
      IDLE_ASSERT(current == me.pref().used());
      return reselect_preferred(me, current);
    }
  }
};

DynDependencyBase::DynDependencyBase(Service& owner)
  : Import(owner)
  , Subscriber({Event::OnCreated})
  , userspace_ptr_(nullptr)
  , pending_(false) {}

Range<ImportList::iterator, ImportList::size_type>
DynDependencyBase::imports() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!pref().ptr() || active().ptr());

  if (active().ptr()) {
    return make_range(ImportList::iterator(active()), {},
                      static_cast<ImportList::size_type>(pref().ptr() ? 2U
                                                                      : 1U));
  } else {
    return {};
  }
}

Range<ImportList::const_iterator, ImportList::size_type>
DynDependencyBase::imports() const noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!pref().ptr() || active().ptr());

  if (active().ptr()) {
    return make_range(ImportList::const_iterator(active()), {},
                      static_cast<ImportList::size_type>(pref().ptr() ? 2U
                                                                      : 1U));
  } else {
    return {};
  }
}

void DynDependencyBase::onPartInit() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  registry_entry_ = owner().root().find(type());
  IDLE_ASSERT(registry_entry_);

  registry_entry_->subscriberAdd(*this);

  if (auto const interfaces = registry_entry_->interfaces()) {
    active().ptr() = reference_of(interfaces.front());
    active().connect();
  } else if (hasDefault()) {
    setDefault();
  } else {
    setSatisfied(false);
  }
}

void DynDependencyBase::onPartDestroy() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!pref().ptr());

  registry_entry_->subscriberDel(*this);

  if (active().ptr()) {
    active().disconnect();
    IDLE_ASSERT(!ImportList::is_inserted_in_any_unsafe(active()));
    active().ptr().reset();
  }
}

void DynDependencyBase::onImportLock() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(isSatisfied());
  IDLE_ASSERT(active().ptr());
  IDLE_ASSERT(active().ptr()->owner().state().isInitializedUnsafe());
  IDLE_ASSERT(active().ptr()->owner().state().isRunning());
  IDLE_ASSERT(!pref().ptr());
  IDLE_ASSERT(!pending_);

  active().ptr().try_acquire();

  store(active().ptr().get());
}

void DynDependencyBase::onImportUnlock() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(active().ptr());
  IDLE_ASSERT(active().ptr()->owner().state().isInitializedUnsafe());
  IDLE_ASSERT(active().ptr()->owner().state().isRunning());

  store(nullptr);

  if (pref().ptr()) {
    active().disconnect();
    active().ptr() = pref().ptr();
    active().connect();

    pref().disconnect();
    ImportList::disconnect(pref());
    pref().ptr().reset();
  } else if (active().ptr()) {
    if (!active().ptr()->owner().state().isDefaultCreated()) {
      // Only disown non default services
      active().ptr().disown();
    }
  }

  pending_ = false;
}

void DynDependencyBase::onSubscribedCreated(Interface& inter) {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  // active contains the preferred service while the
  // service is not running. If the service is started all
  // updates to the preferred service are stored into preferred.
  bool const currently_active = isLocked();

  {
    Ref<Interface>& cur = [&]() -> Ref<Interface>& {
      if (currently_active && pref().ptr()) {
        IDLE_ASSERT(*pref().ptr() > *active().ptr());
        return pref().ptr();
      } else {
        return active().ptr();
      }
    }();

    if (cur && !IsPrioritizedOverCurrent(*cur, inter)) {
      // We aren't interested in a lower priority interface than the current one
      return;
    }
  }

  if (currently_active) {
    IDLE_ASSERT(active().ptr());

    if (pending_) {
      // If a swap is pending do not change the current preferred service!
      return;
    }

    if (!canSwap(*active().ptr(), inter)) {
      // Ignore services we cannot swap to
      return;
    }

    if (pref().ptr()) {
      pref().disconnect();
      ImportList::disconnect(pref());
    }

    pref().ptr() = reference_of(inter);
    ImportList::connect(active(), pref());
    pref().connect();

    if (inter.owner().state().isRunning()) {
      offerSwap();
    }
  } else {
    if (active().ptr()) {
      active().disconnect();
    }

    active().ptr() = reference_of(inter);
    active().connect();
    setSatisfied(true);
  }
}

void DynDependencyBase::onUsedStarted(Export& exp) {
  IDLE_ASSERT(exp.owner().state().isRunning());

  IDLE_DETAIL_LOG_TRACE("{} received onUsedStarted of {}",
                        static_cast<Import&>(*this), exp);

  bool const locked = isLocked();
  IDLE_ASSERT(!locked || exp == *pref().ptr());

  if (locked) {
    offerSwap();
  }
}

void DynDependencyBase::onUsedDestroy(Export& exp) {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  Interface& inter = cast<Interface>(exp);

  // Never destroy a service in active use
  IDLE_ASSERT((active().ptr() != std::addressof(inter)) || !isLocked());
  IDLE_ASSERT((pref().ptr() != std::addressof(inter)) || isLocked());

  if (isLocked()) {
    DynDependencyBaseImpl::reselect_preferred(*this, inter);
  } else {
    IDLE_ASSERT(*active().ptr() == exp);
    bool const is_default = exp.owner().state().isDefaultCreated();
    active().disconnect();

    if (auto const interfaces = registry_entry_->interfaces()) {
      IDLE_ASSERT(interfaces.front() != inter);
      active().ptr() = reference_of(*interfaces.begin());
      active().connect();
    } else {
      IDLE_ASSERT(!ImportList::is_inserted_in_any_unsafe(active()));
      setSatisfied(false);

      active().ptr().reset();

      if (!is_default && hasDefault()) {
        // Create a new default interface only if the current default
        // service was not destroyed while a strong reference existed.
        // This indicates the destruction of the context and all its
        // contained children.
        setDefault();
      }
    }
  }
}

ReleaseReply DynDependencyBase::onReleaseRequest(Export& exp) noexcept {
  IDLE_ASSERT(exp == active().used() || exp == pref().used());

  if (pending_) {
    if (exp == active().used()) {
      return ReleaseReply::Later;
    } else {
      // Theoretically we would need a third variable to stash release
      // requests of the currently preferred interface.
      // For now we handle this by rejecting the release.
      return ReleaseReply::Never;
    }
  }

  Interface& current = cast<Interface>(exp);
  Nullable<Interface> replacement = DynDependencyBaseImpl::get_next_preferred(
      *this, current);
  if (canSwap(current, replacement)) {
    IDLE_ASSERT(!pending_);
    if (exp == pref().used()) {
      DynDependencyBaseImpl::reconnect_preferred(*this, replacement);
      return ReleaseReply::Now;
    } else {
      IDLE_ASSERT(exp == active().used());
      return ReleaseReply::Later;
    }
  } else {
    return ReleaseReply::Never;
  }
}

BitSet<DependenciesFlags>
DynDependencyBase::onInspect(Interface& dep) noexcept {
  (void)dep;
  return {};
}

bool DynDependencyBase::canSwap(Nullable<Interface> from,
                                Nullable<Interface> to) noexcept {
  IDLE_ASSERT(from || to);
  return true;
}

void DynDependencyBase::onSwap(SwapAction<Interface>&& action) noexcept {
  action.apply();
}

bool DynDependencyBase::hasDefault() const noexcept {
  return false;
}

Ref<Interface> DynDependencyBase::createDefault() {
  IDLE_DETAIL_UNREACHABLE();
}

void DynDependencyBase::partName(std::ostream& os) const {
  print(os, FMT_STRING("idle::DynDependency<{}>"), type());
}

#if IDLE_DETAIL_HAS_LOG_LEVEL <= IDLE_DETAIL_LOG_LEVEL_DEBUG
template <typename T>
auto printMaybe(T current) noexcept {
  return printable([current](std::ostream& os) {
    if (current) {
      os << *current;
    } else {
      os << "<null>";
    }
  });
}
#endif

void DynDependencyBase::offerSwap() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!pending_);
  IDLE_ASSERT(pref().ptr());
  IDLE_ASSERT(active().ptr());
  IDLE_ASSERT(*pref().ptr() != *active().ptr());
  IDLE_ASSERT(pref().ptr()->owner().state().isRunning());
  IDLE_ASSERT(isLocked());

  IDLE_DETAIL_LOG_DEBUG("{} offering dependency swap: {} -> {}",
                        static_cast<Import&>(*this), printMaybe(active().ptr()),
                        printMaybe(pref().ptr()));

  pending_ = true;

  // Perform the swap asynchronously on the event loop so we do not mess up
  // the current user list iteration.
  owner().root().event_loop().post([handle = handleOf(*this),
                                    active = active().ptr().get(),
                                    pref = pref().ptr().get()]() mutable {
    if (auto locked = handle.lock()) {
      locked->onSwap(SwapAction<Interface>(std::move(handle), active, pref));
    }
  });
}

bool DynDependencyBase::apply(Handle<DynDependencyBase>&& handle,
                              Interface* from, Interface* to) {

  if (auto locked = handle.lock()) {
    locked->store(to);

    IDLE_DETAIL_LOG_DEBUG("{} Accepted dependency swap: {} -> {}",
                          static_cast<Import&>(*locked), printMaybe(from),
                          printMaybe(to));

    Context& root = locked->owner().root();
    if (root.is_on_event_loop()) {
      locked->finalize();
    } else {
      root.event_loop().post([handle = std::move(handle)] {
        if (auto locked = handle.lock()) {
          locked->finalize();
        }
      });
    }

    return true;
  } else {
    return false;
  }
}

void DynDependencyBase::store(Interface* preferred) noexcept {
  // Updates the user pointers
  userspace_ptr_.store(preferred, std::memory_order_relaxed);
}

void DynDependencyBase::finalize() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(pending_);
  IDLE_ASSERT(pref().ptr());
  IDLE_ASSERT(active().ptr());

  IDLE_DETAIL_LOG_DEBUG("{} finalizing dependency swap: {} -> {}",
                        static_cast<Import&>(*this), printMaybe(active().ptr()),
                        printMaybe(pref().ptr()));

  active().disconnect();
  Ref<Interface> previous(std::move(active().ptr()));
  active().ptr() = reference_of(*pref().ptr());
  active().connect();

  pref().disconnect();
  ImportList::disconnect(pref());
  IDLE_ASSERT(!ImportList::is_inserted_in_any_unsafe(active()));
  IDLE_ASSERT(!ImportList::is_inserted_in_any_unsafe(pref()));
  pref().ptr().reset();

  pending_ = false;
  previous.reset();

  IDLE_ASSERT(!pref().ptr());
  IDLE_ASSERT(imports().size() == 1);

  Interface& latest = registry_entry_->interfaces().front();
  if (latest != *active().ptr()) {
    // During the swap there was a service introduced with a higher priority
    // than the recently swapped in one.
    IDLE_ASSERT(IsPrioritizedOverCurrent(*active().ptr(), latest));

    IDLE_DETAIL_UNREACHABLE(); // NI
  }
}

void DynDependencyBase::setDefault() noexcept {
  IDLE_ASSERT(hasDefault());
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!pref().ptr());

  IDLE_DETAIL_LOG_DEBUG("{} creating default dependency {} for {}",
                        static_cast<Import&>(*this), type(),
                        *static_cast<Part*>(this));

  // We create the default service for this interface on the root context
  auto const created = createDefault();
  IDLE_ASSERT(created);

  created->owner().init();

  // The interface is inserted through the on_interface_create hook.
  IDLE_ASSERT(active().ptr());
  IDLE_ASSERT(active().ptr() == created);
  IDLE_ASSERT(created->owner().state().isInitializedUnsafe());
  IDLE_ASSERT(active().ptr()->owner().state().isDefaultCreated());
  IDLE_ASSERT(active().ptr().is_acquired());
}
} // namespace idle
