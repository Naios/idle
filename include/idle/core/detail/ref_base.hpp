
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

#ifndef IDLE_CORE_DETAIL_REF_BASE_HPP_INCLUDED
#define IDLE_CORE_DETAIL_REF_BASE_HPP_INCLUDED

#include <atomic>
#include <memory>
#include <type_traits>
#include <utility>
#include <idle/core/api.hpp>
#include <idle/core/detail/chaining.hpp>
#include <idle/core/detail/traits.hpp>
#include <idle/core/util/assert.hpp>

namespace idle {
namespace detail {
struct strong_arg_t {};
struct weak_arg_t {};

template <typename, typename>
class ref_base;
template <typename, typename>
class weak_base;

template <typename Tag, typename ControlBlock>
class counter_base {
  template <typename, typename>
  friend class counter_base;

public:
  constexpr counter_base() noexcept = default;
  explicit constexpr counter_base(std::nullptr_t) noexcept
    : block_(nullptr) {}
  explicit constexpr counter_base(ControlBlock* block,
                                  bool add_ref = true) noexcept
    : block_(block) {
    if (block_ && add_ref) {
      block_->increase(Tag{});
    }
  }
  constexpr counter_base(counter_base const& right)
    : counter_base(right.block_) {}
  constexpr counter_base(counter_base&& right) noexcept
    : block_(std::exchange(right.block_, nullptr)) {}
  ~counter_base() {
    if (block_) {
      block_->decrease(Tag{});
    }
  }
  constexpr counter_base& operator=(counter_base const& right) {
    operator=(right.block_);
    return *this;
  }
  constexpr counter_base& operator=(counter_base&& right) noexcept {
    if (auto old = std::exchange(block_, right.block_)) {
      old->decrease(Tag{});
    }
    right.block_ = nullptr;
    return *this;
  }
  constexpr counter_base& operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }

  constexpr counter_base& operator=(ControlBlock* block) {
    if (auto old = std::exchange(block_, block)) {
      old->decrease(Tag{});
    }
    if (block_) {
      block_->increase(Tag{});
    }
    return *this;
  }

  void reset() {
    if (auto old = std::exchange(block_, nullptr)) {
      old->decrease(Tag{});
    }
  }

  void release() {
    block_ = nullptr;
  }

  void increment() noexcept {
    if (block_) {
      block_->increase(Tag{});
    }
  }

  void decrement() {
    if (block_) {
      block_->decrease(Tag{});
    }
  }

  std::size_t count_unsafe() const noexcept {
    if (block_) {
      return block_->count_unsafe(Tag{});
    } else {
      return 0;
    }
  }

  explicit operator bool() const noexcept {
    return bool(block_);
  }

  friend void swap(counter_base& left, counter_base& right) {
    if (&left != &right) {
      auto cache = std::move(left);
      left = std::move(right);
      right = std::move(cache);
    }
  }

protected:
  ControlBlock* block_{nullptr};
};

template <typename ControlBlock>
class strong_counter_base : public counter_base<strong_arg_t, ControlBlock> {
public:
  using counter_base<strong_arg_t, ControlBlock>::counter_base;
  using counter_base<strong_arg_t, ControlBlock>::operator=;
};

template <typename ControlBlock>
class weak_counter_base : public counter_base<weak_arg_t, ControlBlock> {
public:
  using counter_base<weak_arg_t, ControlBlock>::counter_base;
  using counter_base<weak_arg_t, ControlBlock>::operator=;

  strong_counter_base<ControlBlock> lock() const noexcept {
    if (this->block_ && this->block_->try_increase(strong_arg_t{})) {
      return strong_counter_base<ControlBlock>(this->block_, false);
    } else {
      return {};
    }
  }

  bool expired() const noexcept {
    return this->block_ && this->block_->expired();
  }
};

template <typename Parent, typename T>
struct ref_invokeable_base {};
template <typename Parent, typename Ret, typename... Args>
struct ref_invokeable_base<Parent, Ret(Args...)> {
  Ret operator()(Args... args) const {
    auto const fn = static_cast<Parent const*>(this)->ptr_;
    return fn(std::forward<Args>(args)...);
  }
};

template <typename T, typename = void>
struct ref_acquire_base : std::false_type {};
template <typename T, typename ControlBlock>
struct ref_acquire_base<ref_base<T, ControlBlock>,
                        void_t<decltype(std::declval<T*>()->refCounter())>> {

  void try_acquire() noexcept {
    auto* me = static_cast<ref_base<T, ControlBlock>*>(this);
    if (me->ptr_ && !me->is_acquired()) {
      auto* counter = static_cast<strong_counter_base<ControlBlock>*>(me);
      *counter = me->ptr_->refCounter();
    }
  }
};

template <typename T>
struct ptr_trait {
  using pointer_t = T*;
  using const_pointer_t = T const*;
  using reference_t = T&;
  using chain_t = chain_trait<T>;
};
template <typename Ret, typename... Args>
struct ptr_trait<Ret(Args...)> {
  using pointer_t = Ret (*)(Args...);
  using const_pointer_t = pointer_t;
  using reference_t = pointer_t;
  using chain_t = chain_nothing<pointer_t>;
};

template <typename T, typename ControlBlock>
class ref_base : public strong_counter_base<ControlBlock>,
                 public ref_invokeable_base<ref_base<T, ControlBlock>, T>,
                 public ref_acquire_base<ref_base<T, ControlBlock>> {

  template <typename, typename>
  friend class ref_base;
  template <typename, typename>
  friend class weak_base;
  template <typename, typename>
  friend struct ref_invokeable_base;
  template <typename, typename>
  friend struct ref_acquire_base;

  using trait = ptr_trait<T>;
  using chain_t = typename trait::chain_t;

public:
  using pointer_t = typename trait::pointer_t;
  using const_pointer_t = typename trait::const_pointer_t;
  using reference_t = typename trait::reference_t;

  constexpr ref_base() noexcept = default;

  // Constructing
  constexpr ref_base(pointer_t ptr, strong_counter_base<ControlBlock> counter)
    : strong_counter_base<ControlBlock>(std::move(counter))
    , ptr_(ptr) {
    IDLE_ASSERT(!(bool(this->block_) && !bool(this->ptr_)));
  }

  template <
      typename O,
      std::enable_if_t<std::is_convertible<O, pointer_t>::value>* = nullptr>
  constexpr ref_base(O ptr, strong_counter_base<ControlBlock> counter)
    : strong_counter_base<ControlBlock>(std::move(counter))
    , ptr_(ptr) {}

  // Move constructing
  /* implicit */ constexpr ref_base(ref_base<T, ControlBlock>&& right) noexcept
    : strong_counter_base<ControlBlock>(std::move(right))
    , ptr_(std::exchange(right.ptr_, nullptr)) {}

  template <typename O, std::enable_if_t<std::is_convertible<
                            typename ref_base<O, ControlBlock>::pointer_t,
                            pointer_t>::value>* = nullptr>
  /* implicit */ constexpr ref_base(ref_base<O, ControlBlock>&& right) noexcept
    : strong_counter_base<ControlBlock>(std::move(right))
    , ptr_(std::exchange(right.ptr_, nullptr)) {}

  // Copy constructing
  /* implicit */ constexpr ref_base(ref_base<T, ControlBlock> const& right)
    : strong_counter_base<ControlBlock>(right)
    , ptr_(right.ptr_) {}

  template <typename O, std::enable_if_t<std::is_convertible<
                            typename ref_base<O, ControlBlock>::pointer_t,
                            pointer_t>::value>* = nullptr>
  /* implicit */ constexpr ref_base(ref_base<O, ControlBlock> const& right)
    : strong_counter_base<ControlBlock>(right)
    , ptr_(right.ptr_) {}

  // Move assigning
  /* implicit */ constexpr ref_base&
  operator=(ref_base<T, ControlBlock>&& right) noexcept {
    ptr_ = std::exchange(right.ptr_, nullptr);
    strong_counter_base<ControlBlock>::operator=(std::move(right));
    return *this;
  }

  template <typename O, std::enable_if_t<std::is_convertible<
                            typename ref_base<O, ControlBlock>::pointer_t,
                            pointer_t>::value>* = nullptr>
  constexpr ref_base& operator=(ref_base<O, ControlBlock>&& right) {
    ptr_ = std::exchange(right.ptr_, nullptr);
    strong_counter_base<ControlBlock>::operator=(std::move(right));
    return *this;
  }

  // Copy assigning
  constexpr ref_base& operator=(ref_base<T, ControlBlock> const& right) {
    ptr_ = right.ptr_;
    strong_counter_base<ControlBlock>::operator=(right);
    return *this;
  }

  template <typename O, std::enable_if_t<std::is_convertible<
                            typename ref_base<O, ControlBlock>::pointer_t,
                            pointer_t>::value>* = nullptr>
  /* implicit */ constexpr ref_base&
  operator=(ref_base<O, ControlBlock> const& right) {
    ptr_ = right.ptr_;
    strong_counter_base<ControlBlock>::operator=(right);
    return *this;
  }

  // Reset assign
  constexpr ref_base& operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }

  // Pointer comparison
  constexpr bool operator==(std::add_const_t<T>* ptr) const noexcept {
    return ptr_ == ptr;
  }
  constexpr bool operator==(ref_base const& right) const noexcept {
    return ptr_ == right.ptr_;
  }
  constexpr bool operator!=(std::add_const_t<T>* ptr) const noexcept {
    return ptr_ != ptr;
  }
  constexpr bool operator!=(ref_base const& right) const noexcept {
    return ptr_ != right.ptr_;
  }

  constexpr reference_t operator*() const noexcept {
    return *ptr_;
  }

  constexpr typename chain_t::type operator->() const noexcept {
    return chain_t::do_chain(*ptr_);
  }

  constexpr pointer_t get() const noexcept {
    return ptr_;
  }

  explicit operator bool() const noexcept {
    return bool(this->ptr_);
  }

  template <typename Target>
  ref_base<Target, ControlBlock> cast() const& {
    Target* const ptr = static_cast<Target*>(ptr_);
    return ref_base<Target, ControlBlock>(ptr, *this);
  }
  template <typename Target>
  ref_base<Target, ControlBlock> cast() && {
    Target* const ptr = static_cast<Target*>(ptr_);
    return ref_base<Target, ControlBlock>(ptr, std::move(*this));
  }

  template <typename Target>
  ref_base<Target, ControlBlock> pin(Target& target) const& {
    return ref_base<Target, ControlBlock>(std::addressof(target), *this);
  }
  template <typename Target>
  ref_base<Target, ControlBlock> pin(Target& target) && {
    return ref_base<Target, ControlBlock>(std::addressof(target),
                                          std::move(*this));
  }

  void reset() {
    ptr_ = nullptr;
    strong_counter_base<ControlBlock>::reset();
  }

  pointer_t release() noexcept {
    pointer_t ptr = std::exchange(ptr_, nullptr);
    strong_counter_base<ControlBlock>::release();
    return ptr;
  }

  weak_base<T, ControlBlock> weak() const noexcept {
    return weak_base<T, ControlBlock>(ptr_, this->block_);
  }

  bool is_acquired() const noexcept {
    return strong_counter_base<ControlBlock>::operator bool();
  }

  void disown() noexcept {
    strong_counter_base<ControlBlock>::reset();
  }

protected:
  pointer_t ptr_{nullptr};
};

template <typename T, typename ControlBlock>
class weak_base : public weak_counter_base<ControlBlock> {
  template <typename, typename>
  friend class weak_base;

public:
  using pointer_t = typename ptr_trait<T>::pointer_t;

  constexpr weak_base() noexcept = default;

  // Constructing
  /* implicit */ constexpr weak_base(pointer_t ptr, ControlBlock* block,
                                     bool add_ref = true)
    : weak_counter_base<ControlBlock>(block, add_ref)
    , ptr_(ptr) {
    IDLE_ASSERT(!(bool(this->block_) && !bool(this->ptr_)));
  }

  template <
      typename O,
      std::enable_if_t<std::is_convertible<O, pointer_t>::value>* = nullptr>
  /* implicit */ constexpr weak_base(O ptr, ControlBlock* block,
                                     bool add_ref = true)
    : weak_counter_base<ControlBlock>(block, add_ref)
    , ptr_(ptr) {}

  // Move constructing
  /* implicit */ constexpr weak_base(
      weak_base<T, ControlBlock>&& right) noexcept
    : weak_counter_base<ControlBlock>(std::move(right))
    , ptr_(std::exchange(right.ptr_, nullptr)) {}

  template <typename O, std::enable_if_t<std::is_convertible<
                            typename weak_base<O, ControlBlock>::pointer_t,
                            pointer_t>::value>* = nullptr>
  /* implicit */ constexpr weak_base(
      weak_base<O, ControlBlock>&& right) noexcept
    : weak_counter_base<ControlBlock>(std::move(right))
    , ptr_(std::exchange(right.ptr_, nullptr)) {}

  // Copy constructing
  /* implicit */ constexpr weak_base(weak_base<T, ControlBlock> const& right)
    : weak_counter_base<ControlBlock>(right)
    , ptr_(right.ptr_) {}

  template <typename O, std::enable_if_t<std::is_convertible<
                            typename weak_base<O, ControlBlock>::pointer_t,
                            pointer_t>::value>* = nullptr>
  /* implicit */ constexpr weak_base(weak_base<O, ControlBlock> const& right)
    : weak_counter_base<ControlBlock>(right)
    , ptr_(right.ptr_) {}

  // Move assigning
  /* implicit */ weak_base&
  operator=(weak_base<T, ControlBlock>&& right) noexcept {
    ptr_ = std::exchange(right.ptr_, nullptr);
    weak_counter_base<ControlBlock>::operator=(std::move(right));
    return *this;
  }

  template <typename O, std::enable_if_t<std::is_convertible<
                            typename weak_base<O, ControlBlock>::pointer_t,
                            pointer_t>::value>* = nullptr>
  /* implicit */ constexpr weak_base&
  operator=(weak_base<O, ControlBlock>&& right) {
    ptr_ = std::exchange(right.ptr_, nullptr);
    weak_counter_base<ControlBlock>::operator=(std::move(right));
    return *this;
  }

  // Copy assigning
  /* implicit */ constexpr weak_base&
  operator=(weak_base<T, ControlBlock> const& right) {
    ptr_ = right.ptr_;
    weak_counter_base<ControlBlock>::operator=(right);
    return *this;
  }

  template <typename O, std::enable_if_t<std::is_convertible<
                            typename weak_base<O, ControlBlock>::pointer_t,
                            pointer_t>::value>* = nullptr>
  /* implicit */ constexpr weak_base&
  operator=(weak_base<O, ControlBlock> const& right) {
    ptr_ = right.ptr_;
    weak_counter_base<ControlBlock>::operator=(right);
    return *this;
  }

  // Converting
  template <typename O, std::enable_if_t<std::is_convertible<
                            typename ref_base<O, ControlBlock>::pointer_t,
                            pointer_t>::value>* = nullptr>
  /* implicit */ constexpr weak_base(ref_base<O, ControlBlock> const& right) {
    ptr_ = right.ptr_;
    weak_counter_base<ControlBlock>::operator=(right.block_);
  }

  template <typename O, std::enable_if_t<std::is_convertible<
                            typename ref_base<O, ControlBlock>::pointer_t,
                            pointer_t>::value>* = nullptr>
  /* implicit */ constexpr weak_base&
  operator=(ref_base<O, ControlBlock> const& right) {
    ptr_ = right.ptr_;
    weak_counter_base<ControlBlock>::operator=(right.block_);
    return *this;
  }

  /* implicit */ constexpr weak_base(pointer_t ptr,
                                     weak_counter_base<ControlBlock> counter)
    : weak_counter_base<ControlBlock>(std::move(counter))
    , ptr_(ptr) {}

  constexpr weak_base& operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }

  ref_base<T, ControlBlock> lock() const noexcept {
    if (ptr_) {
      if (this->block_) {
        if (auto strong = weak_counter_base<ControlBlock>::lock()) {
          return ref_base<T, ControlBlock>(ptr_, std::move(strong));
        }
      } else {
        return ref_base<T, ControlBlock>(ptr_, {});
      }
    }
    return {};
  }

  bool expired() const noexcept {
    if (ptr_ && !this->block_) {
      return false;
    } else {
      return !ptr_ || weak_counter_base<ControlBlock>::expired();
    }
  }

  void reset() {
    ptr_ = nullptr;
    weak_counter_base<ControlBlock>::reset();
  }

protected:
  pointer_t ptr_{nullptr};
};

template <typename Base>
class atomic_cb : public Base {
public:
  constexpr atomic_cb() noexcept = default;
  atomic_cb(atomic_cb&&) = delete;
  atomic_cb(atomic_cb const&) = delete;
  atomic_cb& operator=(atomic_cb&&) = delete;
  atomic_cb& operator=(atomic_cb const&) = delete;

  auto load(strong_arg_t) noexcept {
    return strong_.load(std::memory_order_acquire);
  }
  auto load(weak_arg_t) noexcept {
    return weak_.load(std::memory_order_acquire);
  }

  void increase(strong_arg_t) noexcept {
    auto const previous = strong_.fetch_add(1U, std::memory_order_acq_rel);
    (void)previous;
    IDLE_ASSERT(previous != 0);
  }
  void increase(weak_arg_t) noexcept {
    weak_.fetch_add(1U, std::memory_order_acq_rel);
  }
  bool try_increase(strong_arg_t) noexcept {
    if (auto strong = strong_.load(std::memory_order_relaxed)) {
      while (!strong_.compare_exchange_weak(strong, strong + 1,
                                            std::memory_order_release,
                                            std::memory_order_relaxed)) {
        if (strong == 0) {
          return false;
        }
      }
      return true;
    } else {
      return false;
    }
  }
  bool expired() const noexcept {
    return strong_.load(std::memory_order_acquire) == 0;
  }
  void decrease(strong_arg_t) noexcept {
    auto const previous = strong_.fetch_sub(1U, std::memory_order_acq_rel);
    IDLE_ASSERT(previous != 0);

    switch (previous) {
      case 1U: {
        this->on_destruct();
        decrease(weak_arg_t{});
        break;
      }
      case 2U: {
        this->on_unique();
        break;
      }
      default: {
        break; // Do nothing
      }
    }
  }
  void decrease(weak_arg_t) noexcept {
    auto const previous = weak_.fetch_sub(1U, std::memory_order_acq_rel);
    IDLE_ASSERT(previous != 0);

    if (previous == 1) {
      IDLE_ASSERT(load(strong_arg_t{}) == 0U);
      this->on_deallocate();
    }
  }

  std::uint_least32_t count_unsafe(strong_arg_t) const noexcept {
    return strong_.load(std::memory_order_acquire);
  }
  std::uint_least32_t count_unsafe(weak_arg_t) const noexcept {
    return weak_.load(std::memory_order_acquire);
  }

private:
  std::atomic_uint_least32_t strong_{1};
  std::atomic_uint_least32_t weak_{1}; // + 1 if strong > 0
};
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_REF_BASE_HPP_INCLUDED
