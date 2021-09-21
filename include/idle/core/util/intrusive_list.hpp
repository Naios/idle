
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

#ifndef IDLE_CORE_UTIL_INTRUSIVE_LIST_HPP_INCLUDED
#define IDLE_CORE_UTIL_INTRUSIVE_LIST_HPP_INCLUDED

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/intrusive_list_node.hpp>
#include <idle/core/util/iterator_facade.hpp>

namespace idle {
namespace detail {
template <typename Itr, typename T>
constexpr bool contains_linear(Itr begin, Itr end, T const& value) noexcept {
  T const* const addr = std::addressof(value);
  for (; begin != end; ++begin) {
    if (std::addressof(*begin) == addr) {
      return true;
    }
  }
  return false;
}
} // namespace detail

template <typename T, typename Tag, typename Node>
class intrusive_forward_list_iterator
  : public iterator_facade<intrusive_forward_list_iterator<T, Tag, Node>,
                           std::forward_iterator_tag, T&> {

  using trait = intrusive_list_trait<std::remove_const_t<T>, Tag, Node>;

public:
  intrusive_forward_list_iterator() = default;
  explicit intrusive_forward_list_iterator(T& current)
    : current_(std::addressof(current)) {}
  explicit intrusive_forward_list_iterator(T* current)
    : current_(current) {}

  bool equal(intrusive_forward_list_iterator const& other) const noexcept {
    return current_ == other.current_;
  }

  void increment() noexcept {
    current_ = trait::get_next(dereference());
  }

  T& dereference() const noexcept {
    IDLE_ASSERT(current_);
    return *current_;
  }

private:
  T* current_{nullptr};
};

namespace detail {
template <typename T>
class intrusive_forward_list_base {
public:
  intrusive_forward_list_base() = default;

protected:
  T* first_{nullptr};
};
template <typename T>
class intrusive_forward_list_base<T&> {
public:
  intrusive_forward_list_base() = delete;
  explicit intrusive_forward_list_base(T*& first) noexcept
    : first_(first) {}

protected:
  T*& first_;
};
} // namespace detail

template <typename T, typename Tag,
          typename Node = intrusive_forward_list_node<
              std::remove_reference_t<T>, Tag>>
class intrusive_forward_list : public detail::intrusive_forward_list_base<T> {
  using trait = intrusive_list_trait<std::remove_reference_t<T>, Tag, Node>;
  using type_t = std::remove_reference_t<T>;

public:
  using node = Node;
  using value_type = type_t&;
  using difference_type = std::ptrdiff_t;
  using pointer = type_t*;
  using reference = value_type;
  using const_reference = type_t const&;
  using iterator = intrusive_forward_list_iterator<type_t, Tag, Node>;
  using const_iterator = intrusive_forward_list_iterator<type_t const, Tag,
                                                         Node>;

  using detail::intrusive_forward_list_base<T>::intrusive_forward_list_base;
  intrusive_forward_list() = default;
  intrusive_forward_list(intrusive_forward_list&&) = delete;
  intrusive_forward_list(intrusive_forward_list const&) = delete;
  intrusive_forward_list& operator=(intrusive_forward_list&&) = delete;
  intrusive_forward_list& operator=(intrusive_forward_list const&) = delete;
  ~intrusive_forward_list() = default;

  constexpr iterator begin() noexcept {
    return iterator{this->first_};
  }
  constexpr iterator end() noexcept {
    return iterator{};
  }
  constexpr const_iterator begin() const noexcept {
    return const_iterator{this->first_};
  }
  constexpr const_iterator end() const noexcept {
    return const_iterator{};
  }
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator{this->first_};
  }
  constexpr const_iterator cend() const noexcept {
    return const_iterator{};
  }

  constexpr bool empty() const noexcept {
    return this->first_ == nullptr;
  }

  void push(value_type value) noexcept {
    IDLE_ASSERT(!trait::get_next(value));
    trait::set_next(value, this->first_);
    this->first_ = std::addressof(value);
  }

  value_type pop() noexcept {
    IDLE_ASSERT(!empty());
    value_type value = front();
    this->next_ = trait::get_next(value);
    trait::set_next(value, nullptr);
    return value;
  }

  constexpr bool contains(const_reference value) const noexcept {
    return detail::contains_linear(begin(), end(), value);
  }

  reference front() noexcept {
    IDLE_ASSERT(!empty());
    return *this->first_;
  }
  const_reference front() const noexcept {
    IDLE_ASSERT(!empty());
    return *this->first_;
  }

  constexpr iterator find(value_type value) noexcept {
    if (contains_unsafe(value)) {
      return {value};
    } else {
      return end();
    }
  }
  constexpr const_iterator find(const_reference value) const noexcept {
    if (contains_unsafe(value)) {
      return {value};
    } else {
      return end();
    }
  }
};

template <typename T, typename Tag,
          typename Node = intrusive_forward_list_node<T, Tag>>
using intrusive_forward_list_view = intrusive_forward_list<T&, Tag, Node>;

template <typename T, typename Tag, typename Node>
class intrusive_list_iterator
  : public iterator_facade<intrusive_list_iterator<T, Tag, Node>,
                           std::bidirectional_iterator_tag, T&> {

  using trait = intrusive_list_trait<std::remove_const_t<T>, Tag, Node>;

public:
  intrusive_list_iterator() = default;
  explicit intrusive_list_iterator(T& current)
    : current_(std::addressof(current)) {}
  explicit intrusive_list_iterator(T* current)
    : current_(current) {}

  bool equal(intrusive_list_iterator const& other) const noexcept {
    return current_ == other.current_;
  }

  void increment() noexcept {
    current_ = trait::get_next(dereference());
  }

  void decrement() noexcept {
    current_ = trait::get_previous(dereference());
  }

  T& dereference() const noexcept {
    IDLE_ASSERT(current_);
    return *current_;
  }

private:
  T* current_{nullptr};
};

namespace detail {
template <typename T, typename SizeType>
class intrusive_list_base;
template <typename T, typename SizeType>
class intrusive_list_base {
  static_assert(!std::is_reference<T>::value, "");
  static_assert(!std::is_reference<SizeType>::value, "");

public:
  using size_type = SizeType;

  intrusive_list_base() = default;

#ifndef NDEBUG
  ~intrusive_list_base() {
    // The nodes should remove itself prior to the destruction
    IDLE_ASSERT(size_ == 0U);
    IDLE_ASSERT(!first_);
    IDLE_ASSERT(!last_);
  }
#endif // NDEBUG

  size_type size() const noexcept {
    return size_;
  }

protected:
  void increase_size() noexcept {
    ++size_;
  }
  void decrease_size() noexcept {
    --size_;
  }

  T* first_{nullptr};
  T* last_{nullptr};

private:
  SizeType size_{0U};
};
template <typename T, typename SizeType>
class intrusive_list_base<T&, SizeType&> {
public:
  using size_type = SizeType;

  intrusive_list_base() = delete;
  explicit intrusive_list_base(T*& first, T*& last, SizeType& size) noexcept
    : first_(first)
    , last_(last)
    , size_(size) {}

  size_type size() const noexcept {
    return size_;
  }

protected:
  void increase_size() noexcept {
    ++size_;
  }
  void decrease_size() noexcept {
    --size_;
  }

  T*& first_;
  T*& last_;

private:
  SizeType& size_;
};
} // namespace detail

template <typename T, typename Tag,
          typename Node = intrusive_list_node<std::remove_reference_t<T>, Tag>,
          typename SizeType = std::size_t>
class intrusive_list : public detail::intrusive_list_base<T, SizeType> {
  using type_t = std::remove_reference_t<T>;
  using trait = intrusive_list_trait<type_t, Tag, Node>;

public:
  using node = Node;
  using difference_type = std::ptrdiff_t;
  using value_type = type_t&;
  using pointer = type_t*;
  using reference = type_t&;
  using const_reference = type_t const&;
  using iterator = intrusive_list_iterator<type_t, Tag, Node>;
  using const_iterator = intrusive_list_iterator<type_t const, Tag, Node>;

  using detail::intrusive_list_base<T, SizeType>::intrusive_list_base;
  intrusive_list() = default;
  intrusive_list(intrusive_list&&) = delete;
  intrusive_list(intrusive_list const&) = delete;
  intrusive_list& operator=(intrusive_list&&) = delete;
  intrusive_list& operator=(intrusive_list const&) = delete;

  iterator begin() noexcept {
    return iterator{this->first_};
  }
  iterator end() noexcept {
    return iterator{};
  }

  const_iterator begin() const noexcept {
    return const_iterator{this->first_};
  }
  const_iterator end() const noexcept {
    return const_iterator{};
  }

  const_iterator cbegin() const noexcept {
    return const_iterator{this->first_};
  }
  const_iterator cend() const noexcept {
    return const_iterator{};
  }

  bool empty() const noexcept {
    IDLE_ASSERT(!this->first_ || this->last_);
    return this->first_ == nullptr;
  }

  void clear() noexcept {
    erase(begin(), end());
    IDLE_ASSERT(empty());
  }

  bool erase(value_type value) noexcept {
    IDLE_ASSERT(!is_inserted_in_any_unsafe(value) ||
                trait::get_previous(value) || is_first(value));
    IDLE_ASSERT(!is_inserted_in_any_unsafe(value) || trait::get_next(value) ||
                is_last(value));

    if (is_inserted_in_any_unsafe(value)) {
      this->decrease_size();

      if (is_first(value)) {
        this->first_ = trait::get_next(value);
      }
      if (is_last(value)) {
        this->last_ = trait::get_previous(value);
      }

      disconnect(value);
      return true;
    } else {
      IDLE_ASSERT(is_first(value) == is_last(value));
      if (is_first(value)) {
        this->first_ = nullptr;
        this->last_ = nullptr;
        this->decrease_size();
        return true;
      }
    }

    IDLE_ASSERT(!contains_unsafe(value));
    IDLE_ASSERT(!trait::get_previous(value) && !is_first(value));
    IDLE_ASSERT(!trait::get_next(value) && !is_last(value));
    return false;
  }
  bool erase(iterator itr) noexcept {
    return erase(*itr);
  }
  void erase(iterator begin, iterator end) noexcept {
    // TODO Improve this
    for (; begin != end;) {
      value_type current = *begin;
      ++begin;
      bool result = erase(current);
      IDLE_ASSERT(result);
      (void)result;
    }
  }

  void push_front(value_type value) noexcept {
    insert_between(value, nullptr, this->first_);
  }

  void push_back(value_type value) noexcept {
    insert_between(value, this->last_, nullptr);
  }

  /// The value is inserted before the given element
  void insert(iterator pos, value_type value) noexcept {
    if (pos == end()) {
      push_back(value);
    } else {
      IDLE_ASSERT(contains_unsafe(*pos));
      insert_between(value, trait::get_previous(*pos), std::addressof(*pos));
    }
  }

  reference front() noexcept {
    IDLE_ASSERT(!empty());
    return *this->first_;
  }
  const_reference front() const noexcept {
    IDLE_ASSERT(!empty());
    return *this->first_;
  }

  reference back() noexcept {
    IDLE_ASSERT(!empty());
    return *this->last_;
  }
  const_reference back() const noexcept {
    IDLE_ASSERT(!empty());
    return *this->last_;
  }

  iterator find(value_type value) noexcept {
    if (contains_unsafe(value)) {
      return {value};
    } else {
      return end();
    }
  }
  const_iterator find(const_reference value) const noexcept {
    if (contains_unsafe(value)) {
      return {value};
    } else {
      return end();
    }
  }

  /// This method only returns safely true if
  /// this node is contained in a list with more than one elements.
  ///
  /// If this method return true the node is definitely inserted,
  /// but one can't assume that this node isn't inserted if this
  /// method returns false since it may be the only element in the list.
  ///
  /// Hence `IDLE_ASSERT(!is_inserted_in_any_unsafe(...))` is always safe to
  /// use.
  static bool is_inserted_in_any_unsafe(const_reference value) noexcept {
    return bool(trait::get_next(value)) || bool(trait::get_previous(value));
  }

  bool contains_unsafe(const_reference value) const noexcept {
    IDLE_ASSERT(is_inserted_in_any_unsafe(value) || !is_first(value) ||
                this->first_ == this->last_);
    return is_inserted_in_any_unsafe(value) || is_first(value);
  }

  bool contains(const_reference value) const noexcept {
    return detail::contains_linear(begin(), end(), value);
  }

  /// Connects two nodes together that don't have a container origin
  static void connect(value_type left, value_type right) noexcept {
    node_insert_between(right, std::addressof(left), nullptr);
  }

  /// Connects two nodes together that don't have a container origin
  static void connect(value_type between, value_type left,
                      value_type right) noexcept {

    node_insert_between(between, std::addressof(left), std::addressof(right));
  }

  /// Disconnects a free il_node that doesn't have a container origin
  static void disconnect(value_type element) noexcept {
    IDLE_ASSERT(is_inserted_in_any_unsafe(element));

    pointer const next = trait::get_next(element);
    pointer const previous = trait::get_previous(element);

    if (previous) {
      trait::set_next(*previous, next);
    }

    if (next) {
      trait::set_previous(*next, previous);
    }

    trait::set_previous(element, nullptr);
    trait::set_next(element, nullptr);
    IDLE_ASSERT(!is_inserted_in_any_unsafe(element));
  }

private:
  bool is_first(const_reference value) const noexcept {
    return std::addressof(value) == this->first_;
  }
  bool is_last(const_reference value) const noexcept {
    return std::addressof(value) == this->last_;
  }
  void insert_between(value_type value, pointer left, pointer right) noexcept {
    IDLE_ASSERT(!is_inserted_in_any_unsafe(value));
    IDLE_ASSERT(!contains(value));

    pointer const ptr = std::addressof(value);

    if (empty()) {
      this->first_ = ptr;
      this->last_ = ptr;
    } else {
      if (right && is_first(*right)) {
        IDLE_ASSERT(!left);
        this->first_ = ptr;
      }

      if (left && is_last(*left)) {
        IDLE_ASSERT(!right);
        this->last_ = ptr;
      }
    }

    this->increase_size();
    node_insert_between(value, left, right);

    IDLE_ASSERT(contains_unsafe(value));
    IDLE_ASSERT(trait::get_previous(value) || is_first(value));
    IDLE_ASSERT(trait::get_next(value) || is_last(value));
  }

  static void node_insert_between(reference node, pointer left,
                                  pointer right) noexcept {
    IDLE_ASSERT(!is_inserted_in_any_unsafe(node));
    IDLE_ASSERT((left != right) || (left == nullptr));
    IDLE_ASSERT(!left || !right ||
                ((trait::get_next(*left) == right) &&
                 (trait::get_previous(*right) == left)));

    if (left) {
      IDLE_ASSERT(trait::get_next(*left) == right);

      trait::set_previous(node, left);
      trait::set_next(*left, std::addressof(node));
    }

    if (right) {
      IDLE_ASSERT(trait::get_previous(*right) == left);

      trait::set_next(node, right);
      trait::set_previous(*right, std::addressof(node));
    }
  }
};

template <typename T, typename Tag, typename Node = intrusive_list_node<T, Tag>,
          typename SizeType = std::size_t>
using intrusive_list_view = intrusive_list<T&, Tag, Node, SizeType&>;
} // namespace idle

#endif // IDLE_CORE_UTIL_INTRUSIVE_LIST_HPP_INCLUDED
