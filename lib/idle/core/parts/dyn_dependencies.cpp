
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

#include <idle/core/dep/format.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/parts/dyn_dependencies.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/nullable.hpp>
#include <idle/core/util/range.hpp>
#include <idle/core/util/upcastable.hpp>

namespace idle {
class DynDependenciesBase::impl {
public:
  static void process_availability(DynDependenciesBase& self,
                                   Interface& inter) {
    auto const flags = self.onInspect(inter);
    if (flags.contains(DependenciesFlags::Ignore)) {
      // If the interface was filtered out do nothing
      return;
    }

    UsageImpl& use = insert(self, inter, flags);
    if (usage_is_connected(use)) {
      // Do nothing
      return;
    }

    if (!self.isLocked()) {
      // Directly use the dependency when the part is unlocked
      usage_connect(self, use);
    } else {
      // If the part is locked already we kindly ask the user to insert
      // it into the user storage or do something else.
      UseReply const reply = self.onUseOffer(inter);

      IDLE_DETAIL_LOG_DEBUG("onUseOffer of {} returned {}",
                            static_cast<Import&>(self), reply);

      switch (reply) {
        case UseReply::UseNowAndInsert: {
          usage_set_lock(use, true);
          usage_connect(self, use);
          usage_insert_into_storage(self, use, inter, StorageUpdate::Auto);
          break;
        }
        case UseReply::UseLater: {
          usage_set_lock(use, true);
          usage_connect(self, use);
          break;
        }
        case UseReply::UseLaterAndNotify:
        case UseReply::UseLaterAndInsert: {
          usage_set_reply(use, reply);
          usage_set_lock(use, true);
          usage_connect(self, use);
          break;
        }
        case UseReply::UseNever: {
          usage_set_reply(use, UseReply::UseNever);
          break;
        }
        default: {
          IDLE_DETAIL_UNREACHABLE();
        }
      }
    }
  }

  static UsageImpl& insert(DynDependenciesBase& self, Interface& inter,
                           BitSet<DependenciesFlags> flags) noexcept {
    auto const res = self.map_.emplace(
        std::piecewise_construct, std::forward_as_tuple(std::addressof(inter)),
        std::forward_as_tuple(self, inter, flags));
    IDLE_ASSERT(res.first->first);
    return res.first->second;
  }

  static Nullable<UsageImpl> find(DynDependenciesBase& self,
                                  Interface& dep) noexcept {

    auto const itr = self.map_.find(std::addressof(dep));
    if (itr != self.map_.end()) {
      return {itr->second};
    } else {
      return {};
    }
  }

  static bool erase(DynDependenciesBase& self, Interface& used) noexcept {
#ifndef NDEBUG
    auto const itr = self.map_.find(std::addressof(used));
    if (itr != self.map_.end()) {
      UsageImpl& uimpl = itr->second;

      IDLE_ASSERT(!usage_is_connected(uimpl));
      IDLE_ASSERT(uimpl.storage_state_ == storage_state_t::erased);
    }
#endif

    return self.map_.erase(std::addressof(used));
  }

  static void usage_set_lock(UsageImpl& me, bool active) noexcept {
    if (active) {
      me.used_.try_acquire();
    } else {
      me.used_.disown();
    }
  }

  static void usage_release(DynDependenciesBase& me, UsageImpl& use) noexcept {
    IDLE_ASSERT(me.owner().root().is_on_event_loop());
    IDLE_ASSERT(use.used_);
    IDLE_ASSERT(!usage_is_connected(use));

    bool const result = erase(me, *use.used_);
    IDLE_ASSERT(result);
    (void)result;
  }

  static bool usage_is_connected(UsageImpl const& me) noexcept {
    return me.owner_.used_.contains_unsafe(me);
  }

  static void usage_connect(DynDependenciesBase& self,
                            UsageImpl& use) noexcept {
    IDLE_ASSERT(!usage_is_connected(use));
    IDLE_ASSERT(!use.isConnected());

    self.used_.push_back(use);
    use.connect(use.is_weak_ ? Usage::UsageType::Weak
                             : Usage::UsageType::Strong);

    IDLE_ASSERT(use.isConnected());
  }

  static void usage_disconnect(DynDependenciesBase& self,
                               UsageImpl& use) noexcept {
    IDLE_ASSERT(usage_is_connected(use));
    IDLE_ASSERT(use.isConnected());
    IDLE_ASSERT(use.storage_state_ == storage_state_t::erased);

    use.disconnect();
    self.used_.erase(use);

    IDLE_ASSERT(!usage_is_connected(use));
    IDLE_ASSERT(!use.isConnected());
  }

  static void usage_insert_into_storage(DynDependenciesBase& self,
                                        UsageImpl& use, Interface& dep,
                                        StorageUpdate update) noexcept {

    if (use.storage_state_ != storage_state_t::inserted) {
      use.storage_state_ = storage_state_t::inserted;

      IDLE_DETAIL_LOG_TRACE("Inserting {} into dependencies_base {} ({})", dep,
                            static_cast<Part&>(self), self.owner());

      self.doInsert(dep, update);
    }
  }

  static void usage_erase_from_storage(DynDependenciesBase& self,
                                       UsageImpl& use, Interface& dep,
                                       StorageUpdate update) noexcept {

    if (use.storage_state_ != storage_state_t::erased) {
      use.storage_state_ = storage_state_t::erased;

      IDLE_DETAIL_LOG_TRACE("Erasing {} from dependencies_base {} ({})", dep,
                            static_cast<Part&>(self), self.owner());

      self.doErase(dep, update);
    }
  }

  static void usage_set_storage_state(UsageImpl& use,
                                      storage_state_t state) noexcept {
    use.storage_state_ = state;
  }

  static UseReply usage_get_reply(UsageImpl const& me) noexcept {
    return me.use_reply_;
  }

  static void usage_set_reply(UsageImpl& me, UseReply reply) noexcept {
    me.use_reply_ = reply;
  }
};

void DynDependenciesBase::UsageImpl::onUsedDestroy(Export& exp) {
  IDLE_ASSERT(user().owner().root().is_on_event_loop());
  IDLE_DETAIL_LOG_DEBUG("Destroying usage of {}", exp);

  IDLE_ASSERT(used_);
  Interface& dep = cast<Interface>(exp);
  IDLE_ASSERT(dep == used());
  IDLE_ASSERT(impl::usage_is_connected(*this));

  impl::usage_erase_from_storage(owner_, *this, dep, StorageUpdate::Implicit);

  impl::usage_disconnect(owner_, *this);
  impl::usage_release(owner_, *this);
}

void DynDependenciesBase::UsageImpl::onUsedStarted(Export& exp) {
  Interface& inter = cast<Interface>(exp);
  auto const use = impl::find(owner_, inter);
  IDLE_ASSERT(use);

  UseReply const previous_reply = impl::usage_get_reply(*use);
  switch (previous_reply) {
    case UseReply::UseLaterAndNotify: {
      IDLE_DETAIL_LOG_DEBUG(
          "Importer {} notified about service {} started late.",
          static_cast<Usage&>(*this), exp.owner());

      UseReply const reply = owner_.onUseOffer(inter);
      IDLE_ASSERT(reply != UseReply::UseLaterAndInsert);
      IDLE_ASSERT(reply != UseReply::UseLaterAndNotify);

      if (reply == UseReply::UseNowAndInsert) {
        impl::usage_insert_into_storage(owner_, *this, inter,
                                        StorageUpdate::Auto);
      }
      break;
    }
    case UseReply::UseLaterAndInsert: {
      impl::usage_insert_into_storage(owner_, *this, inter,
                                      StorageUpdate::Auto);
      break;
    }
    default: {
      break;
    }
  }
}

ReleaseReply
DynDependenciesBase::UsageImpl::onReleaseRequest(Export& exp) noexcept {
  Interface& inter = cast<Interface>(exp);

  ReleaseReply const reply = owner_.onReleaseRequest(inter);
  if (reply == ReleaseReply::Now) {
    IDLE_ASSERT(impl::usage_is_connected(*this));
    auto const use = impl::find(owner_, inter);
    IDLE_ASSERT(use);

    impl::usage_erase_from_storage(owner_, *this, inter, StorageUpdate::Auto);
    impl::usage_disconnect(owner_, *use);
    impl::usage_set_lock(*use, false);
  }

  return reply;
}

DynDependenciesBase::DynDependenciesBase(Service& owner)
  : Import(owner)
  , Subscriber({Event::OnCreated, Event::OnDestroy, Event::OnUnlocked}) {}

DynDependenciesBase::~DynDependenciesBase() {
  IDLE_ASSERT(used_.empty());
}

Range<ImportList::iterator, ImportList::size_type>
DynDependenciesBase::imports() noexcept {
  return make_range(used_.begin(), used_.end(), used_.size());
}

Range<ImportList::const_iterator, ImportList::size_type>
DynDependenciesBase::imports() const noexcept {
  return make_range(used_.begin(), used_.end(), used_.size());
}

void DynDependenciesBase::onPartInit() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!isLocked());

  registry_entry_ = owner().root().find(type());
  IDLE_ASSERT(registry_entry_);

  for (Interface& inter : registry_entry_->interfaces()) {
    this->onSubscribedCreated(inter);
  }

  registry_entry_->subscriberAdd(*this);
}

void DynDependenciesBase::onPartDestroy() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!isLocked());

  registry_entry_->subscriberDel(*this);

  auto const end = used_.end();
  for (auto itr = used_.begin(); itr != end;) {
    // Increment the iterator before calling the user hook because it could
    // modify the intrusive list by removing itself from the list.
    Usage& u = *itr;
    ++itr;

    UsageImpl& uimpl = UsageImpl::from(u);

    impl::usage_erase_from_storage(*this, uimpl, uimpl.used(),
                                   StorageUpdate::Implicit);

    impl::usage_disconnect(*this, uimpl);
    impl::usage_release(*this, uimpl);
  }

  map_.clear();
}

void DynDependenciesBase::onImportLock() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  for (Usage& user : used_) {
    UsageImpl& impl = UsageImpl::from(user);

    if (impl.used().owner().state().isRunning()) {
      impl::usage_set_lock(impl, true);

      impl::usage_insert_into_storage(*this, impl, impl.used(),
                                      StorageUpdate::Implicit);
    } else {
      IDLE_ASSERT(impl.isWeak());

      impl::usage_set_reply(impl, UseReply::UseLaterAndNotify);
      impl::usage_erase_from_storage(*this, impl, impl.used(),
                                     StorageUpdate::Implicit);
    }
  }
}

void DynDependenciesBase::onImportUnlock() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  for (Usage& user : used_) {
    impl::usage_set_lock(UsageImpl::from(user), false);
  }

  for (auto& observed : map_) {
    IDLE_ASSERT(observed.first);

    Interface& dep = *observed.first;
    UsageImpl& use = observed.second;

    if (!impl::usage_is_connected(use)) {
      impl::usage_set_reply(observed.second, UseReply::UseLater);
      impl::usage_connect(*this, use);
    }

    impl::usage_insert_into_storage(*this, use, dep, StorageUpdate::Implicit);
  }
}

void DynDependenciesBase::onSubscribedCreated(Interface& inter) {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(inter.type() == type());

  impl::process_availability(*this, inter);
}

void DynDependenciesBase::onSubscribedDestroy(Interface& inter) {
  impl::erase(*this, inter);
}

void DynDependenciesBase::onSubscribedUnlocked(Interface& inter) {
  // Re-connect to a non connected service after it was unlocked
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(inter.type() == type());

  impl::process_availability(*this, inter);
}

void DynDependenciesBase::partName(std::ostream& os) const {
  print(os, FMT_STRING("idle::dependencies<{}>"), type());
}

/*
void DynDependenciesBase::printPartDetails(std::ostream& os) const {
  Import::printPartDetails(os);

  print(os, FMT_STRING(", {} actively used"), used_.size());
}
*/

void DynDependenciesBase::doAcquireUsage(Interface& dep) noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  // Treat possible issues gracefully, only allow changes while being locked
  if (isLocked()) {
    auto use = impl::find(*this, dep);

    // If the Interface has no entry (possible when filtered out previously)
    // then create an entry to it.
    if (!use) {
      auto const flags = onInspect(dep);
      use = impl::insert(*this, dep, flags);
    }

    // Assume that the user has inserted the dependency into the storage
    impl::usage_set_storage_state(*use, storage_state_t::unknown);

    if (!impl::usage_is_connected(*use)) {
      impl::usage_set_lock(*use, true);
      impl::usage_connect(*this, *use);
    }
  }
}

void DynDependenciesBase::doReleaseUsage(Interface& dep) noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  // Treat possible issues gracefully, only allow changes while being locked
  if (isLocked()) {
    if (auto use = impl::find(*this, dep)) {
      // Assume that the user has erased the dependency from the storage
      impl::usage_set_storage_state(*use, storage_state_t::erased);

      if (impl::usage_is_connected(*use)) {
        impl::usage_disconnect(*this, *use);
        impl::usage_set_lock(*use, false);
      }
    }
  }
}
} // namespace idle
