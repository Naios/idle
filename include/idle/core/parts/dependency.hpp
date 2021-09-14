
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

#ifndef IDLE_CORE_PARTS_DEPENDENCY_HPP_INCLUDED
#define IDLE_CORE_PARTS_DEPENDENCY_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/detail/chaining.hpp>
#include <idle/core/detail/registry_traits.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/intrusive_list.hpp>

namespace idle {
class IDLE_API(idle) DependencyBase : public Import,
                                      protected Subscriber,
                                      protected Usage {

public:
  explicit DependencyBase(Service& owner)
    : Import(owner)
    , Subscriber({Event::OnCreated, Event::OnDestroy})
    , owner_(owner) {}

  using Import::refCounter;
  using Import::weakRefCounter;

  Service& owner() noexcept override {
    return owner_;
  }

  Service const& owner() const noexcept override {
    return owner_;
  }

  Range<ImportList::iterator, ImportList::size_type> imports() noexcept final;
  Range<ImportList::const_iterator, ImportList::size_type>
  imports() const noexcept final;

  bool operator==(DependencyBase const& right) const noexcept {
    return &raw() == &right.raw();
  }

  bool operator!=(DependencyBase const& right) const noexcept {
    return &raw() != &right.raw();
  }

protected:
  virtual Interface::Id type() const noexcept = 0;

  void onPartInit() noexcept override;
  void onPartDestroy() noexcept override;

  void onImportLock() noexcept override;
  void onImportUnlock() noexcept override;

  void onSubscribedCreated(Interface& inter) override;
  void onUsedDestroy(Export& exp) override;

  ReleaseReply onReleaseRequest(Export& exp) noexcept override;

  Import& user() noexcept override {
    return *this;
  }
  Import const& user() const noexcept override {
    return *this;
  }

  Interface& raw() noexcept {
    IDLE_ASSERT(ref_);
    return *ref_;
  }

  Interface const& raw() const noexcept {
    IDLE_ASSERT(ref_);
    return *ref_;
  }

  virtual bool hasDefault() const noexcept;
  virtual Ref<Interface> createDefault(Inheritance inh);

  void partName(std::ostream& os) const override;

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
  void setDefault() noexcept;

  Service& owner_;
  Ref<Interface> ref_;
  Ref<Registry> registry_entry_;
};

/// Implements an immutable and non-changeable dependency on an Interface.
///
/// The depending Interface is valid during the whole lifetime
/// of the depending Service. The depending Interface is allowed to change
/// across different lifetimes of the depending Service.
///
/// It is guaranteed that the depending Interface is kept stable and valid
/// during one lifetime of the Service and therefore the Dependency is
/// fully thread-safe to access from multiple execution contexts during the
/// lifetime of a Service.
template <typename Dep>
class Dependency final : public DependencyBase {
  using chain_trait = detail::chain_trait<Dep>;
  using const_chain_trait = detail::chain_trait<Dep const>;
  static_assert(
      chain_trait::value == const_chain_trait::value,
      "The Dependency has a mismatching operator-> behaviour because of "
      "const correctness. Consider implementing an operator-> for both cases "
      "or qualify operator-> as const!");

public:
  using DependencyBase::DependencyBase;

  typename chain_trait::type operator->() noexcept {
    return chain_trait::do_chain(cast<Dep>(raw()));
  }

  typename const_chain_trait::type operator->() const noexcept {
    return const_chain_trait::do_chain(cast<Dep>(raw()));
  }

  Dep& operator*() noexcept {
    return cast<Dep>(raw());
  }

  Dep const& operator*() const noexcept {
    return cast<Dep>(raw());
  }

protected:
  Interface::Id type() const noexcept override {
    return Dep::id();
  }

  bool hasDefault() const noexcept override {
    return detail::has_create<Dep>::value;
  }

  Ref<Interface> createDefault(Inheritance inh) override {
    return detail::do_create<Dep>(std::move(inh));
  }
};
} // namespace idle

#endif // IDLE_CORE_PARTS_DEPENDENCY_HPP_INCLUDED
