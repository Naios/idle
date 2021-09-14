
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

#ifndef IDLE_CORE_PARTS_STORAGE_SET_STORAGE_HPP_INCLUDED
#define IDLE_CORE_PARTS_STORAGE_SET_STORAGE_HPP_INCLUDED

#include <set>
#include <utility>
#include <idle/core/parts/dependency_types.hpp>
#include <idle/core/service.hpp>

namespace idle {
/// A container based on std::set optimized for ordered insertion and deletion.
///
/// \note This container is optimized for ordered insertion, deletion and
///       lookup. If you need fast iteration speed choose a vector storage.
template <typename Dependency>
class SetStorage {
  using storage_t = std::set<std::reference_wrapper<Dependency>,
                             Interface::GreaterPred>;

public:
  using size_type = typename storage_t::size_type;
  using difference_type = typename storage_t::difference_type;
  using pointer = typename storage_t::pointer;
  using const_pointer = typename storage_t::const_pointer;
  using reference = typename storage_t::reference;
  using const_reference = typename storage_t::const_reference;
  using iterator = typename storage_t::iterator;
  using const_iterator = typename storage_t::const_iterator;
  using reverse_iterator = typename storage_t::reverse_iterator;
  using const_reverse_iterator = typename storage_t::const_reverse_iterator;

  SetStorage() = default;

  bool empty() const noexcept {
    return storage_.empty();
  }
  size_type size() const noexcept {
    return storage_.size();
  }
  iterator begin() noexcept {
    return storage_.begin();
  }
  const_iterator begin() const noexcept {
    return storage_.begin();
  }
  const_iterator cbegin() const noexcept {
    return storage_.cbegin();
  }
  iterator end() noexcept {
    return storage_.end();
  }
  const_iterator end() const noexcept {
    return storage_.end();
  }
  const_iterator cend() const noexcept {
    return storage_.cend();
  }

protected:
  void onInsert(Dependency& dep, StorageUpdate /*type*/) noexcept {
    storage_.insert(dep);
  }
  void onErase(Dependency& dep, StorageUpdate /*type*/) noexcept {
    storage_.erase(dep);
  }

  void onLock(Import& self) noexcept {
    // Do nothing
    (void)self;
  }

  void onUnlock(Import& self) noexcept {
    // Do nothing
    (void)self;
  }

private:
  storage_t storage_;
};
} // namespace idle

#endif // IDLE_CORE_PARTS_STORAGE_SET_STORAGE_HPP_INCLUDED
