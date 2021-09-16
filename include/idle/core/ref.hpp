
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

#ifndef IDLE_CORE_REF_HPP_INCLUDED
#define IDLE_CORE_REF_HPP_INCLUDED

#include <atomic>
#include <memory>
#include <type_traits>
#include <utility>
#include <idle/core/detail/ref_base.hpp>
#include <idle/core/util/meta.hpp>

namespace idle {
namespace detail {
/// A control block which dispatches actions through a vtable
class vblock_t_base {
public:
  constexpr vblock_t_base() noexcept = default;
  virtual ~vblock_t_base() noexcept = default;

  /// Is called when there is only one remaining strong reference left
  virtual void on_unique() noexcept = 0;
  /// Is called when there are zero remaining strong references left
  virtual void on_destruct() noexcept = 0;
  /// Is called when there are zero strong and weak references left
  virtual void on_deallocate() noexcept {
    delete this;
  }
};

using virtual_cb = atomic_cb<vblock_t_base>;

template <typename T, typename Trait>
class vblock_t final : private Trait, public virtual_cb {
public:
  constexpr vblock_t(Trait trait) noexcept
    : Trait(std::move(trait)) {}
  virtual ~vblock_t() noexcept = default;

  template <typename... Args>
  void emplace(Args&&... args) {
    new (&object_) T(std::forward<Args>(args)...);
  }

  void on_unique() noexcept override {
    Trait::on_unique(get());
  }

  void on_destruct() noexcept override {
    Trait::on_destruct(get());
  }

  T* get() {
    return reinterpret_cast<T*>(&object_);
  }

private:
  std::aligned_storage_t<sizeof(T), alignof(T)> object_{};
};

template <typename T>
struct default_block_trait {
  void on_unique(T*) noexcept {}

  void on_destruct(T* obj) noexcept {
    obj->~T();
  }
};

template <typename T, typename = void>
struct has_rc_getters : std::false_type {};
template <typename T>
struct has_rc_getters<T, void_t<decltype(std::declval<T*>()->refCounter()),
                                decltype(std::declval<T*>()->weakRefCounter())>>
  : std::true_type {};

} // namespace detail

using WeakRefCounter = detail::weak_counter_base<detail::virtual_cb>;
using RefCounter = detail::strong_counter_base<detail::virtual_cb>;

template <typename T>
using Ref = detail::ref_base<T, detail::virtual_cb>;

template <typename T>
using NullableRef = detail::ref_base<T, detail::virtual_cb>;

template <typename T>
using WeakRef = detail::weak_base<T, detail::virtual_cb>;

class ReferenceCounted {
  friend struct reference_counted_trait;

public:
  ReferenceCounted() = default;
  ReferenceCounted(ReferenceCounted&&) = delete;
  ReferenceCounted(ReferenceCounted const&) = delete;
  ReferenceCounted& operator=(ReferenceCounted&&) = delete;
  ReferenceCounted& operator=(ReferenceCounted const&) = delete;

  RefCounter refCounter() const noexcept {
    return RefCounter{counter_};
  }
  WeakRefCounter weakRefCounter() const noexcept {
    return WeakRefCounter{counter_};
  }

private:
  detail::virtual_cb* counter_ = nullptr;
};

// Can be used to embed data within the allocated object
template <typename Object, typename Data>
struct joined_allocation_block {
  template <typename... Args>
  explicit joined_allocation_block(Args&&... args)
    : obj(std::forward<Args>(args)...) {}

  // Can't use inheritance here because of possibly final declared Objects
  Object obj;
  Data data;
};

struct reference_counted_trait {
  static void set(void*, detail::virtual_cb*) noexcept {}
  static void set(ReferenceCounted* rc, detail::virtual_cb* counter) {
    rc->counter_ = counter;
  }
  template <typename T, typename D>
  static void set(joined_allocation_block<T, D>* rc,
                  detail::virtual_cb* counter) {
    set(std::addressof(rc->obj), counter);
  }

  static detail::virtual_cb* get(void const*) noexcept {
    return nullptr;
  }
  static detail::virtual_cb* get(ReferenceCounted const* rc) {
    return rc->counter_;
  }
  template <typename T, typename D>
  static detail::virtual_cb* get(joined_allocation_block<T, D> const* rc) {
    return get(std::addressof(rc->obj));
  }
};

template <typename T, typename Trait, typename... Args>
Ref<T> allocate(Trait&& trait, Args&&... args) {
  using type = detail::vblock_t<T, std::decay_t<Trait>>;

  auto raii = std::make_unique<type>(std::forward<Trait>(trait));
  raii->emplace(std::forward<Args>(args)...);

  type* cb = raii.release();
  Ref<T> rc(cb->get(), RefCounter(cb, false));
  reference_counted_trait::set(rc.get(), cb);
  return rc;
}

template <typename T, typename... Args>
auto make_ref(Args&&... args) {
  return allocate<T>(detail::default_block_trait<T>{},
                     std::forward<Args>(args)...);
}

namespace detail {
template <typename T>
T* ptr_of(T* obj) noexcept {
  return obj;
}
template <typename T>
T* ptr_of(T& obj) noexcept {
  return std::addressof(obj);
}
} // namespace detail

template <typename T,
          typename RC = Ref<std::remove_pointer_t<std::remove_reference_t<T>>>>
RC refOf(T&& obj) noexcept {
  return RC(detail::ptr_of(obj), detail::ptr_of(obj)->refCounter());
}
template <typename T, typename V,
          typename RC = Ref<std::remove_pointer_t<std::remove_reference_t<T>>>>
RC refOf(T&& obj, V&& view) noexcept {
  return RC(detail::ptr_of(obj), detail::ptr_of(view)->refCounter());
}

template <typename T,
          typename RC = Ref<std::remove_pointer_t<std::remove_reference_t<T>>>>
RC staticOf(T&& obj) noexcept {
  return RC(detail::ptr_of(obj), {});
}

template <typename T, typename WC = WeakRef<
                          std::remove_pointer_t<std::remove_reference_t<T>>>>
WC weakOf(T&& obj) noexcept {
  return WC(detail::ptr_of(obj), detail::ptr_of(obj)->weakRefCounter());
}
template <
    typename T, typename V,
    typename WC = WeakRef<std::remove_pointer_t<std::remove_reference_t<T>>>>
WC weakOf(T&& obj, V&& view) noexcept {
  return WC(detail::ptr_of(obj), detail::ptr_of(view)->weakRefCounter());
}

template <typename T, typename WC = WeakRef<
                          std::remove_pointer_t<std::remove_reference_t<T>>>>
WC staticWeakOf(T&& obj) noexcept {
  return WC(detail::ptr_of(obj), {});
}

template <typename T>
void refSetStaticLifetime(T&& obj) {
  reference_counted_trait::set(detail::ptr_of(obj), nullptr);
}

template <typename T, typename Parent>
void refSetInheritLifetime(T&& child, Parent&& parent) {
  auto* child_ptr = reference_counted_trait::get(detail::ptr_of(child));
  reference_counted_trait::set(detail::ptr_of(parent), child_ptr);
}
} // namespace idle

#endif // IDLE_CORE_REF_HPP_INCLUDED
