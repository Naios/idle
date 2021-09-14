
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

#include <idle/core/detail/context/registry_impl.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>

namespace idle {
void Subscriber::onSubscribedCreated(Interface& subscribed) {
  (void)subscribed;
}

void Subscriber::onSubscribedDestroy(Interface& subscribed) {
  (void)subscribed;
}

void Subscriber::onSubscribedLock(Interface& subscribed) {
  (void)subscribed;
}

void Subscriber::onSubscribedUnlocked(Interface& subscribed) {
  (void)subscribed;
}

Range<PublishedList::iterator, PublishedList::size_type>
Registry::interfaces() noexcept {
  return RegistryImpl::from(this)->interfacesImpl();
}

Range<PublishedList::const_iterator, PublishedList::size_type>
Registry::interfaces() const noexcept {
  return RegistryImpl::from(this)->interfacesImpl();
}

Interface::Id const& Registry::id() const noexcept {
  return RegistryImpl::from(this)->idImpl();
}

void Registry::subscriberAdd(Subscriber& subscriber) {
  return RegistryImpl::from(this)->subscriberAddImpl(subscriber);
}

void Registry::subscriberDel(Subscriber& subscriber) {
  return RegistryImpl::from(this)->subscriberDelImpl(subscriber);
}
} // namespace idle
