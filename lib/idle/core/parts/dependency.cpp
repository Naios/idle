
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

#include <fmt/compile.h>
#include <fmt/format.h>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
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

static bool
registry_contains_higher_priority(Registry& entry,
                                  Interface const& current) noexcept {
  IDLE_ASSERT(entry.interfaces());
  Interface& first = entry.interfaces().front();

  return (first != current) && (first > current);
}

Range<ImportList::iterator, ImportList::size_type>
DependencyBase::imports() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  if (ref_) {
    return make_range(ImportList::iterator(*this), {},
                      static_cast<ImportList::size_type>(1U));
  } else {
    return {};
  }
}

Range<ImportList::const_iterator, ImportList::size_type>
DependencyBase::imports() const noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  if (ref_) {
    return make_range(ImportList::const_iterator(*this), {},
                      static_cast<ImportList::size_type>(1U));
  } else {
    return {};
  }
}

void DependencyBase::onPartInit() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  registry_entry_ = owner().root().find(type());
  IDLE_ASSERT(registry_entry_);

  registry_entry_->subscriberAdd(*this);

  if (auto const interfaces = registry_entry_->interfaces()) {
    ref_ = reference_of(interfaces.front());
    connect();
  } else if (hasDefault()) {
    setDefault();
  } else {
    setSatisfied(false);
  }
}

void DependencyBase::onPartDestroy() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  registry_entry_->subscriberDel(*this);

  if (ref_) {
    disconnect();

    /*if (ref_.count_unsafe() == 1) {
      ref_->owner().destroy();
    }*/

    ref_.reset();
  }
}

void DependencyBase::onImportLock() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(isSatisfied());
  IDLE_ASSERT(ref_);
  IDLE_ASSERT(ref_->owner().state().isInitializedUnsafe());
  IDLE_ASSERT(ref_->owner().state().isRunning());

  ref_.try_acquire();
}

void DependencyBase::onImportUnlock() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(ref_);
  IDLE_ASSERT(ref_->owner().state().isInitializedUnsafe());
  IDLE_ASSERT(ref_->owner().state().isRunning());

  if (registry_contains_higher_priority(*registry_entry_, *ref_)) {
    setSatisfied(false);
    disconnect();
    ref_ = reference_of(registry_entry_->interfaces().front());
    connect();
    setSatisfied(true);
  } else {
    if (!ref_->owner().state().isDefaultCreated()) {
      // Only disown non default services
      ref_.disown();
    }
  }
}

void DependencyBase::onSubscribedCreated(Interface& inter) {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  if (ref_ && !(inter > *ref_)) {
    // We aren't interested in a lower priority interface than the current one
    return;
  }

  // Never do anything when the dependency is locked
  if (!isLocked()) {
    if (ref_) {
      setSatisfied(false);
      disconnect();
    }

    ref_ = reference_of(inter);
    connect();
    setSatisfied(true);
  }
}

void DependencyBase::onUsedDestroy(Export& exp) {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!isLocked());

  // Never destroy a service in active use
  IDLE_ASSERT((ref_ != std::addressof(cast<Interface>(exp))) || !isLocked());

  IDLE_ASSERT(*ref_ == exp);
  bool const is_default = exp.owner().state().isDefaultCreated();
  setSatisfied(false);
  disconnect();
  ref_.reset();

  if (auto const interfaces = registry_entry_->interfaces()) {
    IDLE_ASSERT(interfaces.front() != cast<Interface>(exp));
    ref_ = reference_of(*interfaces.begin());
    connect();
    setSatisfied(true);
  } else if (!is_default && hasDefault()) {
    // Create a new default interface only if the current default
    // service was not destroyed while a strong reference existed.
    // This indicates the destruction of the context and all its
    // contained children.
    setDefault();
  }
}

ReleaseReply DependencyBase::onReleaseRequest(Export& exp) noexcept {
  (void)exp;
  return ReleaseReply::Never;
}

bool DependencyBase::hasDefault() const noexcept {
  return false;
}

Ref<Interface> DependencyBase::createDefault(Inheritance inh) {
  IDLE_DETAIL_UNREACHABLE();
}

void DependencyBase::partName(std::ostream& os) const {
  fmt::print(os, FMT_STRING("idle::Dependency<{}>"), type());
}

void DependencyBase::setDefault() noexcept {
  IDLE_ASSERT(hasDefault());
  IDLE_ASSERT(owner().root().is_on_event_loop());

  IDLE_DETAIL_LOG_DEBUG("Creating default dependency {} for {}", type(),
                        *static_cast<Part*>(this));

  // We create the default service for this interface on the root context
  auto const created = createDefault(
      Inheritance::weak(owner().root().anchor(),
                        Inheritance::Relation::anchor));
  IDLE_ASSERT(created);

  created->owner().init();

  // The interface is inserted through the on_interface_create hook.
  IDLE_ASSERT(ref_);
  IDLE_ASSERT(ref_ == created);
  IDLE_ASSERT(created->owner().state().isInitializedUnsafe());
  IDLE_ASSERT(ref_->owner().state().isDefaultCreated());
  IDLE_ASSERT(ref_.is_acquired());
}
} // namespace idle
