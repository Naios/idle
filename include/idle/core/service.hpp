
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

#ifndef IDLE_CORE_SERVICE_HPP_INCLUDED
#define IDLE_CORE_SERVICE_HPP_INCLUDED

#include <atomic>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <type_traits>
#include <utility>
#include <idle/core/api.hpp>
#include <idle/core/async.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/detail/chaining.hpp>
#include <idle/core/detail/detection.hpp>
#include <idle/core/detail/service_deleter.hpp>
#include <idle/core/detail/state.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/exception.hpp>
#include <idle/core/fwd.hpp>
#include <idle/core/guid.hpp>
#include <idle/core/ilist.hpp>
#include <idle/core/part.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/bitset.hpp>
#include <idle/core/util/intrusive_list.hpp>
#include <idle/core/util/intrusive_list_node.hpp>
#include <idle/core/util/meta.hpp>
#include <idle/core/util/printable.hpp>
#include <idle/core/util/range.hpp>
#include <idle/core/util/source_location.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/core/views/dyn_cast.hpp>

namespace idle {
namespace detail {
IDLE_API(idle) bool is_on_event_loop(Service const& current) noexcept;
IDLE_API(idle) bool try_use(Service const& current) noexcept;
IDLE_API(idle) void inc_use(Service const& current) noexcept;
IDLE_API(idle) void dec_use(Service const& current) noexcept;
} // namespace detail

/// Is thrown when a cyclic dependency path is encountered during startup
class IDLE_API(idle) cyclic_dependency_exception : public Exception {
public:
  cyclic_dependency_exception();

  char const* what() const noexcept override;
};

/// Is thrown when a required service has an unresolved dependency
class IDLE_API(idle) unresolved_import_exception : public Exception {
public:
  unresolved_import_exception();

  virtual Import& unresolved() noexcept = 0;

  char const* what() const noexcept override;
};

namespace detail {
class IDLE_API(idle) Cluster {
  static constexpr std::uint8_t flag_bits = 3;
  static constexpr std::uint8_t override_bits = 1;
  static constexpr std::uint8_t target_bits = 2;
  static constexpr std::uint8_t running_bits = 32 - flag_bits - target_bits -
                                               override_bits;

public:
  Cluster(bool is_auto_created = false);

  bool is_marked_for_stop_ : 1;
  bool is_marked_for_start_ : 1;
  bool is_auto_created_ : 1;
  /// Describes whether this service shall be kept running
  /// as requested by the user.
  override_t override_ : override_bits;
  /// Represents the state when the promise shall be notified
  target_t target_ : target_bits;
  /// Services running
  std::uint32_t running_ : running_bits;
  /// Outer dependencies missing
  std::uint16_t outer_deps_missing_;
  /// Count of service that want this service to start, is propagates
  /// transitively through the reverse graph.
  std::uint16_t pushes_;
  /// Count of service that want this service to stop, is only propagates to the
  /// next cluster in the dependency tree
  std::uint16_t pulls_;
  /// The guid of the last cluster child
  std::atomic<Guid> guid_;
  /// Is increased after every stop and start
  std::atomic<std::size_t> epoch_;
  /// The promise resolved after cluster transitions
  promise<> promise_;
};

IDLE_API(idle) void setCluster(Service& current, Cluster& c) noexcept;
} // namespace detail

class IDLE_API(idle) Import : public Part {
  friend class Service;
  friend class ServiceImpl;

public:
  using Super = Import;

  explicit Import(Service& owner)
    : Part(owner, Part::Kind::kImport) {}

  virtual Range<ImportList::iterator, ImportList::size_type> imports() noexcept;

  virtual Range<ImportList::const_iterator, ImportList::size_type>
  imports() const noexcept;

  auto importNeighbors() noexcept {
    return neighbors() | views::dyn_cast<Import>();
  }
  auto importNeighbors() const noexcept {
    return neighbors() | views::dyn_cast<Import>();
  }

  static bool classof(Part const& other) noexcept {
    return other.partKind() == Part::Kind::kImport;
  }

  bool isSatisfied() const noexcept;

  /// Returns true when the importer has been started but not stopped yet.
  bool isLocked() const noexcept;

protected:
  /// Is called before the on_start method of the service is called
  ///
  /// After this hook was called it isn't ensured anymore that
  /// linked dependencies required by this importer are started.
  virtual void onImportLock() noexcept;
  /// Is called after the on_stop method of the service was called
  ///
  /// After this hook was called it is ensured again, that
  /// linked dependencies required by this importer are started.
  virtual void onImportUnlock() noexcept;

  void setSatisfied(bool satisfied) noexcept;

private:
  bool satisfied_{true};
};

/// A api can be used to register a service to a registry
/// and require it from other services as dependency.
class IDLE_API(idle) Export : public Part {
  friend class Service;
  friend class Usage;
  friend class ServiceImpl;

public:
  using Super = Export;

  explicit Export(Service& owner, Part::Kind subclass = Part::Kind::kExport)
    : Part(owner, subclass) {}

  /// TODO
  ///
  /// \event_loop
  /// This method should be called from the event loop.
  Range<ExportList::iterator, ExportList::size_type> exports() noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(owner()));
    return make_range(dependent_users_.begin(), dependent_users_.end(),
                      dependent_users_.size());
  }
  /// TODO
  ///
  /// \event_loop
  /// This method should be called from the event loop.
  Range<ExportList::const_iterator, ExportList::size_type>
  exports() const noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(owner()));
    return make_range(dependent_users_.begin(), dependent_users_.end(),
                      dependent_users_.size());
  }

  auto exportNeighbors() noexcept {
    return neighbors() | views::dyn_cast<Export>();
  }
  auto exportNeighbors() const noexcept {
    return neighbors() | views::dyn_cast<Export>();
  }

  /// Returns true when this exporter is available
  bool isAvailable() const noexcept;

  static bool classof(Part const& other) noexcept {
    switch (other.partKind()) {
      case Part::Kind::kExport:
      case Part::Kind::kInterface:
        return true;
      default:
        return false;
    }
  }

private:
  ExportList dependent_users_;
};

/// A api can be used to register a service to a registry
/// and require it from other services as dependency.
class IDLE_API(idle) Interface : public Export, public PublishedList::node {
  friend class Service;
  friend class ServiceImpl;
  friend class Usage;

public:
  using Super = Interface;

  /// Implements a greater callable for interfaces which orders based on its
  /// operator> and on equality based on the address.
  ///
  /// \note This ordering keeps the defined ordering of the interface while
  ///       ensuring uniqueness in ordered containers such as sets.
  struct IDLE_API(idle) GreaterPred {
    bool operator()(Interface const& left,
                    Interface const& right) const noexcept;
  };

  class IDLE_API(idle) Id {
    friend std::hash<Id>;

  public:
    Id() = default;
    explicit Id(StringView name) noexcept
      : name_(std::string(name.begin(), name.end())) {}
    explicit Id(std::string name) noexcept
      : name_(std::move(name)) {}

    Id(Id&&) = default;
    Id(Id const&) = default;
    Id& operator=(Id&&) = default;
    Id& operator=(Id const&) = default;
    ~Id() = default;

    bool operator==(Id const& right) const noexcept {
      return !name_.empty() && (name_ == right.name_);
    }

    bool empty() const noexcept {
      return name_.empty();
    }

    explicit operator bool() const noexcept {
      return !empty();
    }

    friend IDLE_API(idle) std::ostream& operator<<(std::ostream&, Id const&);

  private:
    std::size_t hash() const noexcept;

    // TODO Improve this (pool and pre-hash the string maybe)
    std::string name_;
  };

  explicit Interface(Service& owner)
    : Export(owner, Part::Kind::kInterface) {}

  /// Returns the interf::id of this interface
  virtual Id type() const noexcept = 0;

  /// Returns true when this service has a higher priority than
  /// the other service.
  ///
  /// This can be used to implemented a priority which service gets selected
  /// if only one dependency is requested.
  ///
  /// By default an Interface exported by a non default service
  /// \see service::is_default has a higher priority than one,
  /// exported from a default service.
  ///
  /// \param other The other api to compare shall always have the
  ///              same interf::id as the current one.
  ///
  virtual bool operator>(Interface const& other) const noexcept;

  auto interfaceNeighbors() noexcept {
    return neighbors() | views::dyn_cast<Interface>();
  }
  auto interfaceNeighbors() const noexcept {
    return neighbors() | views::dyn_cast<Interface>();
  }

  Range<PublishedList::iterator> publishedNeighbors() noexcept;
  Range<PublishedList::const_iterator> publishedNeighbors() const noexcept;

  static bool classof(Part const& other) noexcept {
    return other.partKind() == Part::Kind::kInterface;
  }

  Registry& registry() noexcept {
    return *registry_entry_;
  }
  Registry const& registry() const noexcept {
    return *registry_entry_;
  }

protected:
  void partName(std::ostream& os) const override;

private:
  Ref<Registry> registry_entry_;
};

template <typename T, typename = void>
struct is_interface : std::false_type {};

template <typename T>
struct is_interface<T, void_t<decltype(&T::id)>> : std::true_type {};

template <typename T>
decltype(auto) visit(Part& current, T&& visitor) {
  switch (current.partKind()) {
    case Part::Kind::kImport: {
      return std::forward<T>(visitor)(cast<Import>(current));
    }
    case Part::Kind::kExport: {
      return std::forward<T>(visitor)(cast<Export>(current));
    }
    case Part::Kind::kInterface: {
      return std::forward<T>(visitor)(cast<Interface>(current));
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}
template <typename T>
decltype(auto) visit(Part const& current, T&& visitor) {
  switch (current.partKind()) {
    case Part::Kind::kImport: {
      return std::forward<T>(visitor)(cast<Import>(current));
    }
    case Part::Kind::kExport: {
      return std::forward<T>(visitor)(cast<Export>(current));
    }
    case Part::Kind::kInterface: {
      return std::forward<T>(visitor)(cast<Interface>(current));
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

class IDLE_API(idle) Inheritance {
  friend class Service;

public:
  enum class Relation : std::uint8_t { parent, anchor };

private:
  explicit Inheritance(Ref<Part> parent, Relation relation)
    : relation_(relation)
    , parent_(std::move(parent)) {}

public:
  template <typename T,
            void_t<decltype(std::declval<T>().inherit())>* = nullptr>
  /* implicit */ Inheritance(T&& parent) noexcept
    : relation_(Relation::parent)
    , parent_(parent.inherit().parent_) {}

  Inheritance(Inheritance const&) = delete;
  Inheritance(Inheritance&&) = default;
  Inheritance& operator=(Inheritance const&) = delete;
  Inheritance& operator=(Inheritance&&) = default;

  static Inheritance strong(Part& parent,
                            Relation relation = Relation::parent) noexcept;
  static Inheritance weak(Part& parent,
                          Relation relation = Relation::parent) noexcept;

  bool isInCluster() const noexcept;

  bool isAnchored() const noexcept {
    return relation_ == Relation::anchor;
  }

  /// The call to adjust needs to happen inside a templated context such
  /// that the compiler uses this_locality() from the current translation unit.
  Inheritance& anchor(Ref<Locality> loc) noexcept {
    if (relation_ == Relation::anchor) {
      if (loc) {
        parent_ = staticOf(loc->anchor());
      }
    }
    return *this;
  }

private:
  Relation relation_;
  Ref<Part> parent_;
};

enum class Reason { Manual, Implicit };

/// Defines the api for an asynchronous service that provides a
/// specific functionality.
///
/// A service can implement an asynchronous on_start and on_stop method which
/// is invoked accordingly.
class IDLE_API(idle) Service : public ReferenceCounted,
                               public ChildrenList::node {

  friend Registry;
  friend Import;
  friend Export;
  friend class ServiceImpl;
  friend Context;
  friend class Scheduler;
  friend Part;
  friend void detail::setCluster(Service&, detail::Cluster&) noexcept;

public:
  using Super = Service;

  enum class Phase : std::uint8_t {
#define IDLE_DETAIL_FOR_EACH_PHASE(NAME) NAME,
#include <idle/core/detail/state.inl>
  };

  /*implicit*/ Service(Inheritance inh);
  virtual ~Service();

  Service(Service&&) = delete;
  Service(Service const&) = delete;
  Service& operator=(Service&&) = delete;
  Service& operator=(Service const&) = delete;

  /// Initializes the service and calls the corresponding on_init hook.
  ///
  /// \event_loop
  void init();

  /// Destroys the service and calls the corresponding on_destroy hook.
  ///
  /// \event_loop
  void destroy();

  /// Starts the service.
  ///
  /// \throws cyclic_dependency_exception asynchronously when the encountered
  ///         service dependency graph is cyclic and thus the service
  ///         can't be started. Resolve the cyclic dependency manually and
  ///         start the service again afterwards.
  ///
  /// \throws unresolved_import_exception asynchronously when a service
  ///         that was requested to be started has an unresolved dependency.
  ///         This can happen for example when a dependency has no default
  ///         creatable service and there is no service in the system matching
  ///         the requirements.
  ///
  /// \attention Exceptions won't be thrown immediately but instead passed
  ///            asynchronously through the \see continuable_base.
  ///
  /// \thread_safe
  continuable<> start(Reason reason = Reason::Manual);

  /// Stops the service.
  ///
  /// \thread_safe
  continuable<> stop(Reason reason = Reason::Manual);

  /// Observe the service, resolves on next successful status change
  ///
  /// \thread_safe
  continuable<> observe();

  /// Returns the parent of this service.
  ///
  /// \thread_safe
  Part& parent() noexcept {
    return *parent_;
  }
  /// \copydoc parent
  Part const& parent() const noexcept {
    return *parent_;
  }

  /// Returns the context of this service.
  ///
  /// \thread_safe
  Context& root() noexcept;
  /// \copydoc root
  Context const& root() const noexcept;

  /// Returns true when this service is the root context
  ///
  /// \thread_safe
  bool isRoot() const noexcept;

  Guid guid() const noexcept {
    IDLE_ASSERT(cluster_);
    Guid const cur = cluster_->guid_.load(std::memory_order_relaxed);
    return Guid(cur.low(), high_guid_);
  }

  Range<PartList::iterator> parts() noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(*this));
    return {parts_.begin(), parts_.end()};
  }
  Range<PartList::const_iterator> parts() const noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(*this));
    return {parts_.begin(), parts_.end()};
  }

  auto imports() noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(*this));
    return parts_ | views::dyn_cast<Import>();
  }
  auto imports() const noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(*this));
    return parts_ | views::dyn_cast<Import>();
  }

  auto exports() noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(*this));
    return parts_ | views::dyn_cast<Export>();
  }
  auto exports() const noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(*this));
    return parts_ | views::dyn_cast<Export>();
  }

  auto interfaces() noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(*this));
    return parts_ | views::dyn_cast<Interface>();
  }
  auto interfaces() const noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(*this));
    return parts_ | views::dyn_cast<Interface>();
  }

  Range<ChildrenList::iterator> neighbors() noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(*this));
    return make_range(ChildrenList::iterator(this), {});
  }
  Range<ChildrenList::const_iterator> neighbors() const noexcept {
    IDLE_ASSERT(detail::is_on_event_loop(*this));
    return make_range(ChildrenList::const_iterator(this), {});
  }

  class IDLE_API(idle) State final {
    friend class Service;
    explicit State(Service const& me) noexcept
      : me_(&me) {}

  public:
    /// TODO
    ///
    /// \event_loop_consistent
    Phase phase() const noexcept {
      return me_->phase_.load(std::memory_order_acquire);
    }

    /// TODO
    ///
    /// \event_loop_consistent
    bool isLocked() const noexcept {
      return isBetween(Phase::locked, Phase::pending);
    }

    /// Returns true if the service is running and in a usable state.
    ///
    /// \event_loop_consistent
    bool isRunning() const noexcept {
      return phase() == Phase::running;
    }

    /// TODO
    ///
    /// \event_loop_consistent
    bool isUsable() const noexcept {
      return isBetween(Phase::starting, Phase::running);
    }

    /// TODO
    ///
    /// \event_loop_consistent
    bool isPending() const noexcept {
      return phase() == Phase::pending;
    }

    /// TODO
    ///
    /// \event_loop_consistent
    bool isStopped() const noexcept {
      return phase() == Phase::stopped;
    }

    /// TODO
    ///
    /// \event_loop_consistent
    bool isConfigurable() const noexcept {
      return isBetween(Phase::stopped, Phase::locked);
    }

    /// TODO
    ///
    /// \event_loop_consistent
    bool isStarting() const noexcept {
      switch (phase()) {
        case Phase::locked:
        case Phase::starting:
          return true;
        default:
          return false;
      }
    }

    /// TODO
    ///
    /// \event_loop_consistent
    bool isStopping() const noexcept {
      return me_->phase_.load(std::memory_order_acquire) == Phase::stopping;
    }

    /// TODO
    ///
    /// \event_loop
    bool isStartable() const noexcept;

    /// TODO
    ///
    /// \event_loop
    bool isStoppable() const noexcept;

    /// TODO
    ///
    /// \event_loop
    bool isManual() const noexcept;

    /// Returns true if this service represents an auto-created service
    /// usually created through a api::create() method.
    ///
    /// \thread_safe
    bool isDefaultCreated() const noexcept;

    /// Returns true when interfaces of this service are added to the registry
    bool isHidden() const noexcept;

    /// Returns whether the object is initialized but not destroyed yet.
    ///
    /// \event_loop_consistent
    bool isInitializedUnsafe() const noexcept;

    /// Returns whether the object is destroyed.
    ///
    /// \event_loop_consistent
    bool isDestroyedUnsafe() const noexcept;

  private:
    bool is(Phase phase) const noexcept {
      return me_->phase_.load(std::memory_order_acquire) == phase;
    }
    bool isBetween(Phase from_inclusive, //
                   Phase to_inclusive) const noexcept;

    Service const* me_;
  };

  State state() const noexcept {
    return State{*this};
  }

  Epoch epoch() const noexcept {
    return Epoch(cluster_->epoch_.load(std::memory_order_acquire));
  }

  /// Returns a printable object that prints the name of the service
  auto name() const {
    return printable([this](std::ostream& os) {
      name(os);
    });
  }
  /// Writes a printable name of this service to the given std::ostream
  virtual void name(std::ostream& os) const;

  /// Prints this service to the given ostream.
  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os,
                                                 Service const& obj);

  class Stats final {
    friend class Service;
    explicit Stats(Service const& me) noexcept
      : me_(&me) {}

  public:
    auto state() const {
      return printable([this](std::ostream& os) {
        me_->print_state(os);
      });
    }

    auto usage() const {
      return printable([this](std::ostream& os) {
        me_->print_usage(os);
      });
    }

    auto cluster() const {
      return printable([this](std::ostream& os) {
        me_->print_cluster(os);
      });
    }

  private:
    Service const* me_;
  };

  Stats stats() const noexcept {
    return Stats{*this};
  }

  bool operator==(Service const& other) const noexcept {
    return this == &other;
  }
  bool operator!=(Service const& other) const noexcept {
    return this != &other;
  }

protected:
  /// Is called on the event loop after this service was constructed.
  ///
  /// \event_loop
  virtual void onInit();

  /// Is called on the event loop before this service is deconstructed.
  ///
  /// \event_loop
  virtual void onDestroy();

  /// Can be overwritten in user code to provide a custom start logic
  ///
  /// Defaults to a ready continuable which does nothing.
  ///
  /// \event_loop
  virtual continuable<> onStart();

  /// Can be overwritten in user code to provide a custom stop logic
  ///
  /// Defaults to a ready continuable which does nothing.
  ///
  /// \event_loop
  virtual continuable<> onStop();

  /// Can be overwritten in user code to provide a custom setup logic
  ///
  /// onSetup is called for the whole cluster, beginning at the cluster head
  /// down to its children:
  /// head ---> child 1 --> child 1.1
  ///       *-> child 2 --> child 2.1
  ///                   *-> child 2.2
  ///
  /// \event_loop
  virtual void onSetup();

private:
  void print_state(std::ostream& os) const;
  void print_usage(std::ostream& os) const;
  void print_cluster(std::ostream& os) const;

  std::atomic<Phase> phase_;
  std::uint16_t inner_deps_missing_; // Missing inner cluster deps
  Guid::High high_guid_;             // The high part of the service guid
  Ref<Part> parent_;                 // The parent of this service
  PartList parts_;                   // An intrusive forward list of all parts
  mutable std::atomic<std::uint32_t> uses_; // All active uses
  detail::Cluster* cluster_; // All services in the cluster share common data
};

IDLE_API(idle)
std::ostream& operator<<(std::ostream& os, Service::Phase value);

template <typename T>
using is_service = std::is_base_of<Service, unrefcv_t<T>>;

/// Returns a rc to a newly allocated idle::object
/// which was constructed from the given arguments.
///
/// Before deconstruction (when the reference counted pointer)
/// has reached zero, the object::destroy method is called
/// asynchronously on the event loop and then the object is
/// deallocated directly on the event loop.
///
/// This happens in a fifo order such that the order of events is preserved.
template <typename T, typename... Args>
Ref<T> spawn(Inheritance inh, Args&&... args) {
  static_assert(std::is_base_of<Service, T>::value,
                "Can only allocate an idle::service!");

  if (inh.isInCluster()) {
    IDLE_ASSERT(!inh.isAnchored());

    // Allocate the service directly
    return allocate<T, detail::service_deleter>(detail::service_deleter{},
                                                std::move(inh),
                                                std::forward<Args>(args)...);
  } else {
    bool const is_auto_created = inh.isAnchored();

    RefCounter counter;
    if (is_auto_created) {
      auto locality = this_locality();
      counter = static_cast<RefCounter const&>(locality);
      IDLE_ASSERT((!!counter) == locality.is_acquired());
      inh.anchor(std::move(locality));
    }

    // Embed a cluster object into the service allocation block
    using block_t = joined_allocation_block<T, detail::Cluster>;

    Ref<block_t> ptr = allocate<block_t, detail::service_deleter>(
        detail::service_deleter{std::move(counter)}, std::move(inh),
        std::forward<Args>(args)...);

    T& current = ptr->obj;
    detail::Cluster& embedded = ptr->data;
    embedded.is_auto_created_ = is_auto_created;
    detail::setCluster(current, embedded);
    IDLE_ASSERT(refOf(current).is_acquired());
    return Ref<T>(&current, std::move(ptr));
  }
}

namespace detail {
template <typename T, typename... Args>
auto instantiate_service_impl(std::true_type /*has_create*/, Args&&... args) {
  return T::create(std::forward<Args>(args)...);
}
template <typename T, typename... Args>
Ref<T> instantiate_service_impl(std::false_type /*has_create*/,
                                Args&&... args) {
  return spawn<T>(std::forward<Args>(args)...);
}
} // namespace detail

template <typename T, typename... Args>
auto instantiate(Args&&... args) {
  return detail::instantiate_service_impl<T>(detail::has_create<T>{},
                                             std::forward<Args>(args)...);
}

/// Holds a service which is allocated directly inside the object itself
/// if the object T is constructible and doesn't expose a `create` method.
/// Otherwise the object is allocated on the heap.
template <typename T, typename = void>
class Inplace : private detail::Cluster {
  using chain_trait = detail::chain_trait<T>;
  using const_chain_trait = detail::chain_trait<T const>;
  static_assert(
      chain_trait::value == const_chain_trait::value,
      "The type T has a mismatching operator-> behaviour because of "
      "const correctness. Consider implementing an operator-> for both cases "
      "or qualify operator-> as const!");

public:
  template <typename... Args>
  explicit Inplace(Inheritance parent, Args&&... args)
    : detail::Cluster(parent.isAnchored())
    , obj_(std::move(parent.anchor(this_locality())),
           std::forward<Args>(args)...) {
    // Inherit the lifetime from the parent
    refSetInheritLifetime(obj_, obj_.parent());

    // Offer the cluster object to the service
    detail::setCluster(obj_, *static_cast<detail::Cluster*>(this));
  }

  ~Inplace() = default;
  Inplace(Inplace&&) = delete;
  Inplace(Inplace const&) = delete;
  Inplace& operator=(Inplace&&) = delete;
  Inplace& operator=(Inplace const&) = delete;

  T& operator*() noexcept {
    return obj_;
  }
  T const& operator*() const noexcept {
    return obj_;
  }

  typename chain_trait::type operator->() noexcept {
    return chain_trait::do_chain(obj_);
  }

  typename const_chain_trait::type operator->() const noexcept {
    return const_chain_trait::do_chain(obj_);
  }

  T* get() noexcept {
    return std::addressof(obj_);
  }
  T const* get() const noexcept {
    return std::addressof(obj_);
  }

private:
  T obj_;
};
template <typename T>
class Inplace<T, std::enable_if_t<detail::has_create<T>::value>> {
  using chain_trait = detail::chain_trait<T>;
  using const_chain_trait = detail::chain_trait<T const>;
  static_assert(
      chain_trait::value == const_chain_trait::value,
      "The type T has a mismatching operator-> behaviour because of "
      "const correctness. Consider implementing an operator-> for both cases "
      "or qualify operator-> as const!");

public:
  template <typename... Args>
  explicit Inplace(Args&&... args)
    : obj_(instantiate<T>(std::forward<Args>(args)...)) {}

  ~Inplace() = default;
  Inplace(Inplace&&) = delete;
  Inplace(Inplace const&) = delete;
  Inplace& operator=(Inplace&&) = delete;
  Inplace& operator=(Inplace const&) = delete;

  T& operator*() noexcept {
    return *obj_;
  }
  T const& operator*() const noexcept {
    return *obj_;
  }

  typename chain_trait::type operator->() noexcept {
    return chain_trait::do_chain(static_cast<T&>(*obj_));
  }

  typename const_chain_trait::type operator->() const noexcept {
    return const_chain_trait::do_chain(static_cast<T const&>(*obj_));
  }

  T* get() noexcept {
    return obj_.get();
  }
  T const* get() const noexcept {
    return obj_.get();
  }

private:
  Ref<T> obj_;
};

/// Defines the result of a \see onReleaseRequest
enum class ReleaseReply : std::uint8_t {
  Never, //< The usage is never released due to this request
  Now,   //< The usage was released due to the request already
  Later  //< The usage will be released later (asynchronously)
};

class IDLE_API(idle) Usage : public ExportList::node, public ImportList::node {
  friend class ServiceImpl;
  friend class DefaultRegistryEntry;

  enum class Flags : std::uint8_t;

public:
  explicit Usage() = default;
  virtual ~Usage() = default;
  Usage(Usage&&) = delete;
  Usage(Usage const&) = delete;
  Usage& operator=(Usage&&) = delete;
  Usage& operator=(Usage const&) = delete;

  /// Returns the api which is used by this user
  ///
  /// \event_loop
  virtual Export& used() noexcept = 0;
  /// \copydoc used
  virtual Export const& used() const noexcept = 0;

  /// Returns the extension which is the actual user
  ///
  /// \event_loop
  virtual Import& user() noexcept = 0;
  /// \copydoc user
  virtual Import const& user() const noexcept = 0;

  bool isConnected() const noexcept;

  /// Returns true when the usage is weak and its depending service is allowed
  /// to be started without the dependent service being running.
  bool isWeak() const noexcept;

  Range<ExportList::iterator> used_neighbors() noexcept {
    return make_range(ExportList::iterator(this), {});
  }
  Range<ExportList::const_iterator> used_neighbors() const noexcept {
    return make_range(ExportList::const_iterator(this), {});
  }

  Range<ImportList::iterator> user_neighbors() noexcept {
    return make_range(ImportList::iterator(this), {});
  }
  Range<ImportList::const_iterator> user_neighbors() const noexcept {
    return make_range(ImportList::const_iterator(this), {});
  }

  enum class UsageType : std::uint8_t { Strong, Weak };

  /// TODO
  ///
  /// \event_loop
  void connect(UsageType type = UsageType::Strong);

  /// TODO
  ///
  /// \event_loop
  void disconnect();

  /// TODO
  ///
  /// \event_loop
  bool requestRelease() noexcept;

  /// Prints this usage to the given ostream.
  friend std::ostream& operator<<(std::ostream& os, Usage const& obj) {
    obj.print_usage_details(os);
    return os;
  }

  /// Prints this usage name to the given ostream.
  auto name() const {
    return printable([this](std::ostream& os) {
      print_usage_name(os);
    });
  }

  auto details() const {
    return printable([this](std::ostream& os) {
      print_usage_details(os);
    });
  }

  bool operator==(Usage const& other) const noexcept {
    return this == &other;
  }

protected:
  /// Is called when the usage is requested to be disconnected
  ///
  /// \returns release_response_t::release_never when the usage will not be
  ///          release due to the request. Otherwise \see release_response_t.
  ///
  /// \note The usage can be released asynchronously,
  ///       however it must be guaranteed that this happens in the near future
  ///       or immediately.
  virtual ReleaseReply onReleaseRequest(Export& exp) noexcept;

  /// TODO
  ///
  /// \event_loop
  virtual void onUsedDestroy(Export& exp);

  /// Is called when the user is locked already and the used service was started
  ///
  /// \event_loop
  virtual void onUsedStarted(Export& exp);

  /// Prints this usage to the given std::ostream
  void print_usage_details(std::ostream& os) const;
  /// Prints this usage name to the given std::ostream
  void print_usage_name(std::ostream& os) const;

private:
  BitSet<Flags> flags_;
};
} // namespace idle

namespace std {
template <>
struct hash<idle::Guid> {
  size_t operator()(idle::Guid const& id) const noexcept {
    return hash<idle::Guid::Value>{}(id.value());
  }
};
template <>
struct hash<idle::Interface::Id> {
  size_t operator()(idle::Interface::Id const& id) const noexcept {
    return id.hash();
  }
};
} // namespace std

#endif // IDLE_CORE_SERVICE_HPP_INCLUDED
