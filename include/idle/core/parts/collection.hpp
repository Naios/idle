
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

#ifndef IDLE_CORE_PARTS_COLLECTION_HPP_INCLUDED
#define IDLE_CORE_PARTS_COLLECTION_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/range.hpp>

namespace idle {

/// Represents an extension which references its children in a non owning way.
class IDLE_API(idle) Collection : public Export {
public:
  using Export::Export;
  virtual ~Collection();

  Range<ChildrenList::iterator, ChildrenList::size_type>
  children() noexcept override {
    return make_range(children_.begin(), children_.end(), children_.size());
  }

  Range<ChildrenList::const_iterator, ChildrenList::size_type>
  children() const noexcept override {
    return make_range(children_.begin(), children_.end(), children_.size());
  }

  Inheritance inherit() noexcept {
    return Inheritance::strong(*this);
  }

  void clear();

protected:
  void onPartInit() noexcept override;
  void onPartDestroy() noexcept override;

  void onChildInit(Service& child) override;
  void onChildDestroy(Service& child) override;

  void partName(std::ostream& os) const override;

private:
  ChildrenList children_;
};
} // namespace idle

#endif // IDLE_CORE_PARTS_COLLECTION_HPP_INCLUDED
