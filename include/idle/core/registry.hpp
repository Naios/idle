
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

#ifndef IDLE_CORE_REGISTRY_HPP_INCLUDED
#define IDLE_CORE_REGISTRY_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/context.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/bitset.hpp>
#include <idle/core/util/intrusive_list.hpp>

namespace idle {
class Subscriber;

namespace tags {
/// Tags the intrusive of registers subscribers for a Interface::id
struct Subscribers {};
} // namespace tags

using SubscriberList = intrusive_list<Subscriber, tags::Subscribers>;

class IDLE_API(idle) Subscriber : public SubscriberList::node {
public:
  enum class Event : std::uint8_t {
    OnCreated,
    OnDestroy,
    OnLock,
    OnUnlocked,
  };

  explicit Subscriber(BitSet<Event> filter = BitSet<Event>::all())
    : filter_(filter) {}
  virtual ~Subscriber() = default;

  void callSubscribedCreated(Interface& inter) {
    if (filter_.contains(Event::OnCreated)) {
      onSubscribedCreated(inter);
    }
  }

  void callSubscribedDestroyed(Interface& inter) {
    if (filter_.contains(Event::OnDestroy)) {
      onSubscribedDestroy(inter);
    }
  }

  void callSubscribedLock(Interface& inter) {
    if (filter_.contains(Event::OnLock)) {
      onSubscribedLock(inter);
    }
  }

  void callSubscribedUnlock(Interface& inter) {
    if (filter_.contains(Event::OnUnlocked)) {
      onSubscribedUnlocked(inter);
    }
  }

protected:
  /// TODO
  ///
  /// \event_loop
  virtual void onSubscribedCreated(Interface& subscribed);

  /// TODO
  ///
  /// \event_loop
  virtual void onSubscribedDestroy(Interface& subscribed);

  /// TODO
  ///
  /// \event_loop
  virtual void onSubscribedLock(Interface& subscribed);

  /// TODO
  ///
  /// \event_loop
  virtual void onSubscribedUnlocked(Interface& subscribed);

private:
  BitSet<Event> const filter_;
};

/// Represents the extensions depending on a single interf::id
class IDLE_API(idle) Registry : public ReferenceCounted {
  friend class RegistryImpl;
  Registry() = default;

public:
  Range<PublishedList::iterator, PublishedList::size_type>
  interfaces() noexcept;
  Range<PublishedList::const_iterator, PublishedList::size_type>
  interfaces() const noexcept;

  Interface::Id const& id() const noexcept;

  void subscriberAdd(Subscriber& subscriber);
  void subscriberDel(Subscriber& subscriber);
};
} // namespace idle

#endif // IDLE_CORE_REGISTRY_HPP_INCLUDED
