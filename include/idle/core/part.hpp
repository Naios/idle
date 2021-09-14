
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

#ifndef IDLE_CORE_PART_HPP_INCLUDED
#define IDLE_CORE_PART_HPP_INCLUDED

#include <cstddef>
#include <iosfwd>
#include <idle/core/api.hpp>
#include <idle/core/ilist.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/printable.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
class IDLE_API(idle) Epoch {
  friend class Service;

  explicit constexpr Epoch(std::size_t counter) noexcept
    : counter_(counter) {}

public:
  constexpr Epoch(Epoch const&) noexcept = default;
  constexpr Epoch& operator=(Epoch const&) noexcept = default;

  constexpr bool operator==(Epoch right) const noexcept {
    return counter_ == right.counter_;
  }
  constexpr bool operator!=(Epoch right) const noexcept {
    return counter_ == right.counter_;
  }

  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os, Epoch epoch);

private:
  std::size_t counter_;
};

class IDLE_API(idle) Part : public PartList::node {
  friend class Service;
  friend class Import;
  friend class Export;
  friend class Interface;

public:
  using Super = Part;

  enum class Kind : std::uint8_t { kImport = 0, kExport = 1, kInterface = 2 };

  explicit Part(Service& owner, Kind kind);
  virtual ~Part() = default;

  Part(Part&&) = delete;
  Part(Part const&) = delete;
  Part& operator=(Part&&) = delete;
  Part& operator=(Part const&) = delete;

  virtual Service& owner() noexcept = 0;
  virtual Service const& owner() const noexcept = 0;

  Range<PartList::iterator> neighbors() noexcept {
    return make_range(PartList::iterator(this), {});
  }
  Range<PartList::const_iterator> neighbors() const noexcept {
    return make_range(PartList::const_iterator(this), {});
  }

  std::uint8_t partOffset() const noexcept;

  RefCounter refCounter() const noexcept;
  WeakRefCounter weakRefCounter() const noexcept;

  Epoch epoch() const noexcept;

  virtual Range<ChildrenList::iterator, ChildrenList::size_type>
  children() noexcept;
  virtual Range<ChildrenList::const_iterator, ChildrenList::size_type>
  children() const noexcept;

  bool operator==(Part const& other) const noexcept {
    return this == &other;
  }
  bool operator!=(Part const& other) const noexcept {
    return this != &other;
  }

  /// Prints this parts name to the given ostream.
  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os,
                                                 Part const& obj);

  auto partName() const {
    return printable([this](std::ostream& os) {
      partName(os);
    });
  }
  /// Prints this part name to the given std::ostream
  virtual void partName(std::ostream& os) const;

  Kind partKind() const noexcept {
    return kind_;
  }

protected:
  /// Is called when the part shall be initialized on the event loop.
  ///
  ///\event_loop_consistent
  virtual void onPartInit() noexcept;
  /// Is called when the part shall be destroyed on the event loop.
  ///
  /// \thread_safe
  virtual void onPartDestroy() noexcept;

  /// Is called when a child is initialized
  ///
  /// \event_loop
  virtual void onChildInit(Service& child);
  /// Is called when a child is destroyed
  ///
  /// \event_loop
  virtual void onChildDestroy(Service& child);

private:
  Kind const kind_;
};
} // namespace idle

#endif // IDLE_CORE_PART_HPP_INCLUDED
