
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

#ifndef IDLE_CORE_DETAIL_CONTEXT_REGISTRY_IMPL_HPP_INCLUDED
#define IDLE_CORE_DETAIL_CONTEXT_REGISTRY_IMPL_HPP_INCLUDED

#include <unordered_map>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/range.hpp>
#include <idle/core/util/upcastable.hpp>

namespace idle {
class RegistryManager : public Export {
public:
  using Export::Export;

  Ref<Registry> findImpl(Interface::Id const& id);

  Range<ChildrenList::iterator, ChildrenList::size_type>
  children() noexcept override {
    return make_range(auto_created_services_.begin(),
                      auto_created_services_.end(),
                      auto_created_services_.size());
  }

  Range<ChildrenList::const_iterator, ChildrenList::size_type>
  children() const noexcept override {
    return make_range(auto_created_services_.begin(),
                      auto_created_services_.end(),
                      auto_created_services_.size());
  }

  void onRegistryEntryDestroy(Interface::Id const& id);

protected:
  void onPartDestroy() noexcept override;

  void onChildInit(Service& child) override;
  void onChildDestroy(Service& child) override;

  void partName(std::ostream& os) const override;

private:
  std::unordered_map<Interface::Id, WeakRef<Registry>> entries_;
  ChildrenList auto_created_services_;
};

/// Represents the extensions depending on a single interf::id
class RegistryImpl : public Registry, public Upcastable<RegistryImpl> {
public:
  RegistryImpl(Ref<RegistryManager> owner, Interface::Id id)
    : owner_(std::move(owner))
    , id_(std::move(id)) {}

  ~RegistryImpl();

  Range<PublishedList::iterator, PublishedList::size_type>
  interfacesImpl() noexcept;
  Range<PublishedList::const_iterator, PublishedList::size_type>
  interfacesImpl() const noexcept;

  Interface::Id const& idImpl() const noexcept;

  void subscriberAddImpl(Subscriber& subscriber);
  void subscriberDelImpl(Subscriber& subscriber);

  void onInterfaceCreate(Interface& inter);
  void onInterfaceStart(Interface& inter);
  void onInterfaceDestroy(Interface& inter);
  void onInterfaceLock(Interface& inter);
  void onInterfaceUnlock(Interface& inter);

private:
  Ref<RegistryManager> owner_;
  Interface::Id const id_;

  PublishedList interfaces_;
  SubscriberList subscribers_;
};
} // namespace idle

#endif // IDLE_CORE_DETAIL_CONTEXT_REGISTRY_IMPL_HPP_INCLUDED
