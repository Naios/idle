
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

#ifndef IDLE_CORE_CONTEXT_HPP_INCLUDED
#define IDLE_CORE_CONTEXT_HPP_INCLUDED

#include <cstddef>
#include <iosfwd>
#include <type_traits>
#include <idle/core/api.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/fwd.hpp>
#include <idle/core/parts/container.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/bitset.hpp>
#include <idle/core/util/executor_facade.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
class RegistryImplementation;

class IDLE_API(idle) Context : public Implements<Container>,
                               public Locality,
                               private ExecutorFacade<Context> {
  friend class ContextImpl;
  friend class ExecutorFacade<Context>;

  explicit Context()
    : Implements<Container>(Inheritance::weak(*this)) {}

public:
  using executor_type = ExecutorFacade<Context>;

  /// Returns the event_loop executor which is used for dispatching work
  /// on a single thread.
  ///
  /// \attention The event loop will always be dispatched from a single thread,
  ///            but the input order of work isn't guaranteed
  ///            to be preserved (work queued from the event loop thread
  ///            will be dispatched directly)!
  executor_type& event_loop() noexcept {
    return *this;
  }

  /// Returns true if the current thread is the event loop thread.
  bool is_on_event_loop() const noexcept;

  /// The parent of the context is always the context itself.
  using Service::parent;

  /// Runs the context, this will block the current thread
  /// until stop is called.
  ///
  /// By default this will run the internal threadpool with a thread count
  /// which size matches the hardware concurrency.
  int run();

  /// Updates all outdated or unhealthy services in order to be bring them into
  /// its correct intended state again.
  continuable<> update();

  /// Stops the context and all registered child objects,
  /// and lets run return with the given exit code.
  continuable<> stop(int exit_code = 0);

  /// Attempts to find the service with the given guid
  ///
  /// \attention This method needs to be called from the event loop!
  Ref<Service> lookup(Guid guid) const noexcept;

  /// Returns the registry to the given Interface
  Ref<Registry> find(Interface::Id const& id);

  /// \copydoc find
  template <typename Interface>
  Ref<Registry> find() noexcept {
    static_assert(is_interface<Interface>::value,
                  "T does not implement Interface::id()!");
    return find(Interface::id());
  }

  /// Creates a new context
  static Ref<Context> create();

  using Implements<Container>::operator==;
  using Implements<Container>::operator!=;

  bool verify(std::ostream& os) noexcept;

private:
  bool can_dispatch_inplace() const noexcept;
  void queue(work work) noexcept;

  IDLE_SERVICE
};

/// A Persistent represents a Service with an infinite lifetime that
/// persists as long as the application is running.
///
/// A persistently wrapped Service will be optimized such that
/// the internal reference counter won't be used anymore because
/// Persistent lifetime is assumed by the system.
///
/// The Persistent can be used to provide a static set of services in the
/// main application as shown below:
/// ```cpp
/// Persistent<Context> root;
/// Persistent<my_service> permanent_service(*root);
/// ```
///
/// \attention Never use this class in a plugin or a service which possibly
///            can be deleted during the lifetime of the application!
template <typename T>
class Persistent final : public Inplace<T> {
public:
  template <typename... Args>
  explicit Persistent(Args&&... args)
    : Inplace<T>(std::forward<Args>(args)...) {
    T* const obj = Inplace<T>::get();

    // Erase the internal self pointer to the control block
    refSetStaticLifetime(obj);

    // Queue the service for initialization
    obj->root().event_loop().dispatch([obj] {
      obj->init();
    });
  }

  ~Persistent() {
    IDLE_ASSERT(Inplace<T>::get()->state().isDestroyedUnsafe());
  }
};

namespace internal {
enum class InternalLogLevel : std::uint8_t {
  trace = 0,
  debug = 1,
  error = 2,
  disabled = 3
};

/// Sets the internal log level of the idle library
///
/// \note The library can offer varying internal logging support depending on
///       compiler optimization. It is not always guaranteed that the enabled
///       level is also supported.
IDLE_API(idle)
void setInternalLog(
    InternalLogLevel level = InternalLogLevel::disabled) noexcept;
} // namespace internal
} // namespace idle

#endif // IDLE_CORE_CONTEXT_HPP_INCLUDED
