
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

#ifndef IDLE_CORE_DETAIL_PARTS_USES_IMPL_HPP_INCLUDED
#define IDLE_CORE_DETAIL_PARTS_USES_IMPL_HPP_INCLUDED

#include <idle/core/service.hpp>
#include <idle/core/util/assert.hpp>

namespace idle {
namespace detail {
template <typename T>
class UsesExport : public Inplace<T>, public Export {
public:
  template <typename... Args>
  explicit UsesExport(Inheritance inh, Args&&... args)
    : Inplace<T>(std::move(inh), std::forward<Args>(args)...)
    , Export(static_cast<Service&>(*Inplace<T>::get())) {}

  Service& owner() noexcept override {
    return *Inplace<T>::get();
  }
  Service const& owner() const noexcept override {
    return *Inplace<T>::get();
  }
};

class UseImport : protected Import, protected Usage {
public:
  explicit UseImport(Service& owner)
    : Import(owner)
    , owner_(owner) {}

  Import& user() noexcept override {
    return *this;
  }

  Import const& user() const noexcept override {
    return *this;
  }

protected:
  Range<ImportList::iterator, ImportList::size_type>
  imports() noexcept override {
    if (used().exports()) {
      return make_range(ImportList::iterator(static_cast<Usage&>(*this)), {},
                        static_cast<ImportList::size_type>(1U));
    } else {
      return {};
    }
  }

  Range<ImportList::const_iterator, ImportList::size_type>
  imports() const noexcept override {
    if (used().exports()) {
      return make_range(ImportList::const_iterator(
                            static_cast<Usage const&>(*this)),
                        {}, static_cast<ImportList::size_type>(1U));
    } else {
      return {};
    }
  }

  void onPartInit() noexcept override {
    if (!used().owner().state().isInitializedUnsafe()) {
      used().owner().init();
    }

    IDLE_ASSERT(used().owner() != user().owner());
    connect();
  }
  void onPartDestroy() noexcept override {
    if (used().owner().state().isInitializedUnsafe()) {
      IDLE_ASSERT(used().exports());
      disconnect();
      used().owner().destroy();
    }
  }
  void onUsedDestroy(Export& exp) override {
    IDLE_ASSERT(exp == used());

    if (exp.exports()) {
      disconnect();
    }
  }

  Service& owner() noexcept override {
    return owner_;
  }
  Service const& owner() const noexcept override {
    return owner_;
  }

private:
  Service& owner_;
};
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_PARTS_USES_IMPL_HPP_INCLUDED
