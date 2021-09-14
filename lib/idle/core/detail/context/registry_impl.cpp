
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

#include <algorithm>
#include <ostream>
#include <idle/core/async.hpp>
#include <idle/core/context.hpp>
#include <idle/core/detail/context/context_impl.hpp>
#include <idle/core/detail/context/registry_impl.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/service_impl.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/assert.hpp>

namespace idle {
Ref<Registry> RegistryManager::findImpl(Interface::Id const& id) {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!!id);

  auto const itr = entries_.find(id);
  if (itr != entries_.end()) {
    if (auto entry = itr->second.lock()) {
      return entry;
    } else {
      entry = make_ref<RegistryImpl>(refOf(this), id);
      itr->second = entry;
      return entry;
    }
  } else {
    auto entry = make_ref<RegistryImpl>(refOf(this), id);
    entries_.insert(std::make_pair(id, entry));
    return entry;
  }
}

Interface::Id const& RegistryImpl::idImpl() const noexcept {
  return id_;
}

void RegistryManager::onChildInit(Service& child) {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!auto_created_services_.contains_unsafe(child));

  IDLE_DETAIL_LOG_DEBUG("Adding default created service '{}' to registry",
                        child);

  auto_created_services_.push_back(child);
}

void RegistryManager::onChildDestroy(Service& child) {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(auto_created_services_.contains_unsafe(child));

  IDLE_DETAIL_LOG_DEBUG("Removing default created service '{}' from registry",
                        child);

  bool const result = auto_created_services_.erase(child);
  (void)result;
  IDLE_ASSERT(result);
}

void RegistryManager::partName(std::ostream& os) const {
  os << "idle::Registry";
}

void RegistryManager::onRegistryEntryDestroy(Interface::Id const& id) {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  auto const itr = entries_.find(id);
  if ((itr != entries_.end()) && itr->second.expired()) {
    entries_.erase(itr);
  }
}

void RegistryManager::onPartDestroy() noexcept {
  while (!auto_created_services_.empty()) {
    Service& current = auto_created_services_.front();
    current.destroy();
  }

  IDLE_ASSERT(auto_created_services_.empty());
}

RegistryImpl::~RegistryImpl() {
  // Auto cleanup the entry on destruction
  owner_->owner().root().event_loop().dispatch([weak = owner_.weak(),
                                                id = this->id_ // MSVC issue
  ]() mutable {
    if (auto owner = weak.lock()) {
      owner->onRegistryEntryDestroy(id);
    }
  });
}

Range<PublishedList::iterator, PublishedList::size_type>
RegistryImpl::interfacesImpl() noexcept {
  IDLE_ASSERT(owner_->owner().root().is_on_event_loop());
  return make_range(interfaces_.begin(), interfaces_.end(), interfaces_.size());
}

Range<PublishedList::const_iterator, PublishedList::size_type>
RegistryImpl::interfacesImpl() const noexcept {
  return make_range(interfaces_.begin(), interfaces_.end(), interfaces_.size());
}

void RegistryImpl::subscriberAddImpl(Subscriber& subscriber) {
  IDLE_ASSERT(owner_->owner().root().is_on_event_loop());
  IDLE_ASSERT(!subscribers_.contains_unsafe(subscriber));
  subscribers_.push_back(subscriber);
}

void RegistryImpl::subscriberDelImpl(Subscriber& subscriber) {
  IDLE_ASSERT(owner_->owner().root().is_on_event_loop());
  subscribers_.erase(subscriber);
}

template <typename T, typename E>
void insert_sorted(T&& list, E& entity) {
  // Insert the service in sorted order such that the service which has a
  // higher priority comes first in the list.
  list.insert(std::lower_bound(list.begin(), list.end(), entity,
                               Interface::GreaterPred{}),
              entity);
}

void RegistryImpl::onInterfaceCreate(Interface& inter) {
  IDLE_ASSERT(owner_->owner().root().is_on_event_loop());
  IDLE_ASSERT(inter.type() == id());
  IDLE_ASSERT(!interfaces_.contains_unsafe(inter));

  insert_sorted(interfaces_, inter);

  for (Subscriber& sub : subscribers_) {
    sub.callSubscribedCreated(inter);
  }
}

template <typename Iterable, typename T>
void stable_for_each(Iterable& iterable, T&& callable) noexcept {
  auto const begin = iterable.begin();
  auto const end = iterable.end();

  for (auto itr = begin; itr != end;) {
    // Increment the iterator before calling the user hook because it could
    // modify the intrusive list by removing itself from the list.
    auto& value = *itr;
    ++itr;

    callable(value);
  }
}

void RegistryImpl::onInterfaceStart(Interface& inter) {
  IDLE_ASSERT(owner_->owner().root().is_on_event_loop());

  auto const exported = inter.exports();
  stable_for_each(exported, [&](Usage& u) {
    // Only call the on_used_started hook when the user is locked already
    // and which means the current service is post loaded.
    if (u.user().isLocked()) {
      ServiceImpl::used_started(u, inter);
    }
  });
}

void RegistryImpl::onInterfaceDestroy(Interface& inter) {
  IDLE_ASSERT(owner_->owner().root().is_on_event_loop());

  IDLE_DETAIL_LOG_DEBUG("Calling subscriber::on_interface_destroy hooks of {}",
                        inter);

  interfaces_.erase(inter);

  auto const exported = inter.exports();
  stable_for_each(exported, [&](Usage& u) {
    ServiceImpl::used_destroy(u, inter);
  });

  for (Subscriber& sub : subscribers_) {
    sub.callSubscribedDestroyed(inter);
  }
}

void RegistryImpl::onInterfaceLock(Interface& inter) {
  for (Subscriber& sub : subscribers_) {
    sub.callSubscribedLock(inter);
  }
}

void RegistryImpl::onInterfaceUnlock(Interface& inter) {
  for (Subscriber& sub : subscribers_) {
    sub.callSubscribedUnlock(inter);
  }
}
} // namespace idle
