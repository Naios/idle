
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

#include <array>
#include <exception>
#include <limits>
#include <memory>
#include <ostream>
#include <tuple>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <idle/core/casting.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/algorithm.hpp>
#include <idle/core/detail/context/context_impl.hpp>
#include <idle/core/detail/context/garbage_collector.hpp>
#include <idle/core/detail/context/scheduler.hpp>
#include <idle/core/detail/exception_handling.hpp>
#include <idle/core/detail/hash.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/rtti.hpp>
#include <idle/core/detail/service_impl.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/guid.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/panic.hpp>
#include <idle/core/util/predicate.hpp>
#include <idle/core/views/filter.hpp>

namespace idle {
static_assert(std::is_trivially_move_assignable<Guid>::value, "");
static_assert(std::is_trivially_move_constructible<Guid>::value, "");
static_assert(std::is_trivially_copy_assignable<Guid>::value, "");
static_assert(std::is_trivially_copy_constructible<Guid>::value, "");

using phase_t = Service::Phase;
using override_t = detail::override_t;
using target_t = detail::target_t;

cyclic_dependency_exception::cyclic_dependency_exception() = default;

char const* cyclic_dependency_exception::what() const noexcept {
  return "A cyclic reference was detected in the system!";
}

unresolved_import_exception::unresolved_import_exception() = default;

char const* unresolved_import_exception::what() const noexcept {
  return "All services were started but some are still unsatisfied!";
}

std::ostream& operator<<(std::ostream& os, Part const& obj) {
  return os << Guid::of(obj) << ' ' << obj.partName();
}

std::ostream& operator<<(std::ostream& os, Epoch epoch) {
  return os << epoch.counter_;
}

namespace detail {
bool is_on_event_loop(Service const& current) noexcept {
  return current.root().is_on_event_loop();
}

bool try_use(Service const& current) noexcept {
  return ServiceImpl::try_use(current);
}

void inc_use(Service const& current) noexcept {
  ServiceImpl::inc_use(current);
}

void dec_use(Service const& current) noexcept {
  ServiceImpl::dec_use(current);
}

Cluster::Cluster(bool is_auto_created)
  : is_marked_for_stop_{false}
  , is_marked_for_start_{false}
  , is_auto_created_(is_auto_created)
  , override_{override_t::none}
  , target_{target_t::none}
  , running_{0U}
  , outer_deps_missing_{0U}
  , pushes_{0U}
  , pulls_{0U}
  , epoch_{0U} {}

void setCluster(Service& current, Cluster& c) noexcept {
  if ((current.cluster_ == nullptr) && !isa<Import>(current.parent())) {
    current.cluster_ = &c;
  }
}
} // namespace detail

Part::Part(Service& owner, Kind kind)
  : kind_(kind) {
  IDLE_ASSERT(!owner.state().isInitializedUnsafe());
  owner.parts_.push(*this);
}

RefCounter Part::refCounter() const noexcept {
  return owner().refCounter();
}

WeakRefCounter Part::weakRefCounter() const noexcept {
  return owner().weakRefCounter();
}

Range<ChildrenList::iterator, ChildrenList::size_type>
Part::children() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  return {};
}

Range<ChildrenList::const_iterator, ChildrenList::size_type>
Part::children() const noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  return {};
}

Epoch Part::epoch() const noexcept {
  return owner().epoch();
}

std::uint8_t Part::partOffset() const noexcept {
  std::size_t const size = distance(owner().parts().begin(),
                                    neighbors().begin());
  // We only allow max(uint8) - 1 parts per service
  IDLE_ASSERT(size < std::numeric_limits<Guid::Offset>::max());
  return std::uint8_t(size) + 1U;
}

void Part::onPartInit() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
}

void Part::onPartDestroy() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
}

void Part::onChildInit(Service& child) {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  (void)child;
}

void Part::onChildDestroy(Service& child) {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  (void)child;
}

void Part::partName(std::ostream& os) const {
  detail::print_class_type(os, *this);
}

Range<ImportList::iterator, ImportList::size_type> Import::imports() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  return {};
}

Range<ImportList::const_iterator, ImportList::size_type>
Import::imports() const noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  return {};
}

void Import::onImportLock() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
}

void Import::onImportUnlock() noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
}

bool Import::isSatisfied() const noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  return satisfied_;
}

bool Import::isLocked() const noexcept {
  return owner().state().isLocked();
}

void Import::setSatisfied(bool satisfied) noexcept {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  if (satisfied != satisfied_) {
    if (satisfied) {
      satisfied_ = true;
      ServiceImpl::on_import_satisfied(owner(), *this);
    } else {
      satisfied_ = false;
      ServiceImpl::on_import_unsatisfied(owner(), *this);
    }
  }
}

Inheritance Inheritance::strong(Part& parent, Relation relation) noexcept {
  return Inheritance(refOf(parent), relation);
}

Inheritance Inheritance::weak(Part& parent, Relation relation) noexcept {
  return Inheritance(staticOf(std::addressof(parent)), relation);
}

bool Inheritance::isInCluster() const noexcept {
  return isa<Import>(*parent_);
}

Service::Service(Inheritance inh)
  : phase_(Phase::uninitialized)
  , inner_deps_missing_(0U)
  , high_guid_(Guid{}.high())
  , parent_(std::move(inh.parent_))
  , uses_(0U)
  , cluster_(nullptr) {

  IDLE_ASSERT(isa<Export>(*parent_) || isa<Import>(*parent_));

  // Services exported by a cluster member that is not the head:
  // IDLE_ASSERT(!isa<Export>(*parent_) || is_cluster_head(parent_->owner()));
}

Service::~Service() {
  IDLE_ASSERT(state().isDestroyedUnsafe() && "on_destroy was not called!");
}

#ifndef NDEBUG
static bool is_reachable_from_parent(Service& me) noexcept {
  for (Service& component : me.parent().children()) {
    if (component == me) {
      return true;
    }
  }
  return false;
}
#endif // NDEBUG

static Guid next_child_guid(Guid current) noexcept {
  return Guid(current.low(), current.high() + 1);
}

void Service::init() {
  IDLE_ASSERT(root().is_on_event_loop());
  IDLE_ASSERT(phase_ == Phase::uninitialized);

  ContextImpl& context = ContextImpl::from(root());

  if (!isRoot() && isa<Import>(parent())) {
    IDLE_ASSERT(cluster_ == nullptr);
    cluster_ = parent().owner().cluster_;

    // Retrieve the current high guid and increment the next cluster guid
    Guid const current = cluster_->guid_.load(std::memory_order_acquire);
    Guid const next = next_child_guid(current);

    high_guid_ = next.high();
    cluster_->guid_.store(next, std::memory_order_release);
  } else {
    IDLE_ASSERT(cluster_);

    // Allocate a low guid from the recycler
    Guid::Low const low = context.allocate_guid(*this);
    Guid const current = Guid(low);
    high_guid_ = current.high();
    cluster_->guid_.store(current, std::memory_order_release);
  }

  detail::garbage_collector::on_service_init(*this);

  ServiceImpl::do_transition_phase_silent(*this, Phase::uninitialized,
                                          Phase::on_init);

  IDLE_DETAIL_LOG_DEBUG("Calling service::on_init of {}", *this);
  this->onInit();

  ServiceImpl::do_transition_phase_silent(*this, Phase::on_init,
                                          Phase::stopped);

  parent_->onChildInit(*this);
  IDLE_ASSERT((isRoot() || is_reachable_from_parent(*this)) &&
              "The service is not reachable from its parent!");

  ServiceImpl::on_service_add(*this);

  for (Interface& inter : interfaces()) {
    ServiceImpl::interface_init(inter, context);
  }

  for (Import& imp : imports()) {
    imp.onPartInit();
  }

  for (Export& exp : exports()) {
    exp.onPartInit();
  }

  for (Interface& inter : interfaces()) {
    ServiceImpl::interface_publish(inter);
  }

  IDLE_DETAIL_LOG_DEBUG("Initialized service {}", *this);

  ContextImpl& cimpl = ContextImpl::from(root());
  cimpl.associated_scheduler().on_service_init(*this);
  cimpl.call_on_service_init(*this);
}

void Service::destroy() {
  IDLE_ASSERT(root().is_on_event_loop());

  if (!state().isInitializedUnsafe()) {
    // Never destroy uninitialized or destroyed services
    return;
  }

  IDLE_ASSERT(cluster_);
  IDLE_ASSERT(!root().state().isDestroyedUnsafe());
  IDLE_ASSERT(state().isInitializedUnsafe());
  IDLE_ASSERT(state().isStopped() && "Attempted to destroy a running service!");
  IDLE_ASSERT(!parent().owner().state().isDestroyedUnsafe());
  IDLE_ASSERT(uses_.load() == 0U);

  IDLE_DETAIL_LOG_DEBUG("Destroying service {}", *this);

  if (is_cluster_head(*this)) {
    // Cancel the promise directly before the service gets invalidated
    ServiceImpl::transition_target_set(*this, target_t::none);
  }

  ContextImpl& cimpl = ContextImpl::from(root());
  cimpl.call_on_service_destroy(*this);

  for (Interface& inter : interfaces()) {
    ServiceImpl::interface_revoke(inter);
  }

  for (Export& exp : exports()) {
    exp.onPartDestroy();
  }

  for (Import& imp : imports()) {
    imp.onPartDestroy();
  }

  for (Interface& inter : interfaces()) {
    ServiceImpl::interface_destroy(inter);
  }

  ServiceImpl::on_service_del(*this);

  parent_->onChildDestroy(*this);

  ContextImpl::from(root()).associated_scheduler().on_service_destroy(*this);

  ServiceImpl::do_transition_phase_silent(*this, Phase::stopped,
                                          Phase::on_destroy);

  IDLE_DETAIL_LOG_DEBUG("Calling service::on_destroy of {}", *this);
  this->onDestroy();

  if (!isa<Import>(parent())) {
    Guid const current = cluster_->guid_.load(std::memory_order_acquire);
    cluster_->guid_.store(Guid{}, std::memory_order_release);

    // Recycle the low guid
    ContextImpl::from(&root())->recycle_guid(*this, current.low());
  }

  ServiceImpl::do_transition_phase_silent(*this, Phase::on_destroy,
                                          Phase::destroyed);

  detail::garbage_collector::on_service_destroy(*this);

#ifndef NDEBUG
  if (is_cluster_head(*this)) {
    // IDLE_ASSERT(!cluster_->is_marked_for_start_); // Does not need to hold
    IDLE_ASSERT(!cluster_->is_marked_for_stop_);
    IDLE_ASSERT(cluster_->override_ == override_t::none);
    IDLE_ASSERT(cluster_->target_ == target_t::none);
    IDLE_ASSERT(!cluster_->promise_);
    IDLE_ASSERT(!cluster_->running_);
    // IDLE_ASSERT(cluster_->outer_deps_missing_ <= 1U /*parent*/);
  }
#endif

  cluster_ = nullptr;
}

continuable<> Service::start(Reason reason) {
  return ServiceImpl::request_start(*this, reason);
}

continuable<> Service::stop(Reason reason) {
  return ServiceImpl::request_stop(*this, reason);
}

continuable<> Service::observe() {
  return ServiceImpl::request_observe(*this);
}

void Service::name(std::ostream& os) const {
  detail::print_class_type(os, *this);
}

void Service::print_state(std::ostream& os) const {
  os << phase_.load(std::memory_order_acquire);
}

void Service::print_usage(std::ostream& os) const {
  print(os, FMT_STRING("in (cluster): -{}, ~out: {}"), inner_deps_missing_,
        uses_.load(std::memory_order_relaxed));
}

std::ostream& operator<<(std::ostream& os, phase_t value) {
  switch (value) {
#define IDLE_DETAIL_FOR_EACH_PHASE(NAME)                                       \
  case phase_t::NAME:                                                          \
    os << #NAME;                                                               \
    break;
#include <idle/core/detail/state.inl>
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
  return os;
}

static auto print_yn(bool set) {
  return printable([set](std::ostream& os) {
    if (set) {
      os.put('Y');
    } else {
      os.put('N');
    }
  });
}

void Service::print_cluster(std::ostream& os) const {
  IDLE_ASSERT(cluster_);

  print(os,
        FMT_STRING("epoch: {}, dep mis: -{}, running: {}, pulls: {}, pushes: "
                   "{}, ovr: {}, "
                   "target: {}, stop: {}, start: {}"),
        cluster_->epoch_.load(std::memory_order_relaxed),
        static_cast<std::uint16_t>(cluster_->outer_deps_missing_),
        static_cast<std::uint16_t>(cluster_->running_), cluster_->pulls_,
        cluster_->pushes_, static_cast<override_t>(cluster_->override_),
        static_cast<target_t>(cluster_->target_),
        print_yn(cluster_->is_marked_for_stop_),
        print_yn(cluster_->is_marked_for_start_));
}

void Service::onInit() {
  IDLE_ASSERT(root().is_on_event_loop());
  IDLE_ASSERT(phase_ == Phase::on_init);
}

void Service::onDestroy() {
  IDLE_ASSERT(root().is_on_event_loop());
  IDLE_ASSERT(phase_ == Phase::on_destroy);
}

continuable<> Service::onStart() {
  return make_ready_continuable();
}

continuable<> Service::onStop() {
  return make_ready_continuable();
}

void Service::onSetup() {
  IDLE_ASSERT(root().is_on_event_loop());
}

Context& Service::root() noexcept {
  Service* current = this;
  Service* old;

  do {
    old = std::exchange(current, &(current->parent().owner()));
  } while (current != old);

  return *static_cast<Context*>(current);
}

Context const& Service::root() const noexcept {
  Service const* current = this;
  Service const* old;

  do {
    old = std::exchange(current, &(current->parent().owner()));
  } while (current != old);

  return *static_cast<Context const*>(current);
}

bool Service::isRoot() const noexcept {
  return parent_->owner() == *this;
}

bool Service::State::isStartable() const noexcept {
  IDLE_ASSERT(me_->root().is_on_event_loop());

  return (me_->inner_deps_missing_ == 0) &&
         (is(Phase::locked) ||
          (is(Phase::stopped) && me_->cluster_->outer_deps_missing_ == 0U));
}

bool Service::State::isStoppable() const noexcept {
  IDLE_ASSERT(me_);
  return isRunning() && ServiceImpl::has_no_external_uses(
                            *me_, me_->uses_.load(std::memory_order_relaxed));
}

bool Service::State::isManual() const noexcept {
  IDLE_ASSERT(me_);
  return me_->cluster_->override_ == override_t::start;
}

bool Service::State::isDefaultCreated() const noexcept {
  IDLE_ASSERT(me_);
  return me_->cluster_->is_auto_created_;
}

bool Service::State::isHidden() const noexcept {
  IDLE_ASSERT(me_);
  return isa<Import>(me_->parent());
}

bool Service::State::isInitializedUnsafe() const noexcept {
  IDLE_ASSERT(me_);

  switch (me_->phase_) {
    case Phase::uninitialized:
    case Phase::destroyed:
    case Phase::on_destroy:
      return false;
    default:
      return true;
  }
}

bool Service::State::isDestroyedUnsafe() const noexcept {
  IDLE_ASSERT(me_);

  switch (me_->phase_) {
    case Phase::uninitialized:
    case Phase::destroyed:
      return true;
    default:
      return false;
  }
}

bool Service::State::isBetween(Phase from_inclusive, //
                               Phase to_inclusive) const noexcept {
  using type_t = std::underlying_type_t<Phase>;
  auto const current = static_cast<type_t>(phase());
  IDLE_ASSERT(static_cast<type_t>(from_inclusive) <=
              static_cast<type_t>(to_inclusive));
  return (static_cast<type_t>(from_inclusive) <= current) &&
         (static_cast<type_t>(to_inclusive) >= current);
}

bool Interface::GreaterPred::operator()(Interface const& left,
                                        Interface const& right) const noexcept {

  IDLE_ASSERT(left.type() == right.type());
  IDLE_ASSERT(left.owner().root() == right.owner().root());

  if (&left == &right) {
    return false;
  } else {
    return UniqueGreaterPred<Interface>{}(left, right);
  }
}

std::size_t Interface::Id::hash() const noexcept {
  std::size_t seed = 0U;
  detail::hash_range_combine(seed, name_.begin(), name_.end());
  return seed;
}

std::ostream& operator<<(std::ostream& os, Service const& obj) {
  if (!obj.state().isDestroyedUnsafe()) {
    return os << Guid::of(obj) << ' ' << obj.name();
  } else {
    return os << ":uninitialized:" << ' ' << obj.name();
  }
}

std::ostream& operator<<(std::ostream& os, idle::Interface::Id const& id) {
  if (id.name_.empty()) {
    return os << "<unknown>";
  } else {
    return os << id.name_;
  }
}

enum class Usage::Flags : std::uint8_t { Connected, Weak, ReleaseRequested };

bool Usage::isConnected() const noexcept {
  return flags_.contains(Flags::Connected);
}

bool Usage::isWeak() const noexcept {
  return flags_.contains(Flags::Weak);
}

void Usage::connect(UsageType type) {
  IDLE_ASSERT(user().owner().root().is_on_event_loop());
  IDLE_ASSERT(!isConnected());
  IDLE_ASSERT(!used().dependent_users_.contains_unsafe(*this));
  IDLE_ASSERT(flags_.empty());

  // We disallow the target to be in the same cluster as the source
  // for preventing circular internal dependencies.
  // This needs to be checked in advance.
  IDLE_DETAIL_LOG_DEBUG("Connecting usage {} to {}", user().owner(),
                        used().owner());

  IDLE_PANIC(!ServiceImpl::is_in_same_cluster(user().owner(), used().owner()),
             fmt::format(FMT_STRING("Attempted to connect a circular usage "
                                    "between Service '{}' to itself!"),
                         user().owner()));

  flags_.insert(Flags::Connected);

  if (type == UsageType::Weak) {
    flags_.insert(Flags::Weak);
  }

  Export& used_export = used();
  ServiceImpl::dependent_add(used_export, *this);
  IDLE_ASSERT(used_export.dependent_users_.contains_unsafe(*this));

  ServiceImpl::on_usage_connected(*this);
  ContextImpl::from(used_export.owner().root()).call_on_usage_connect(*this);
}

void Usage::disconnect() {
  IDLE_ASSERT(user().owner().root().is_on_event_loop());
  IDLE_ASSERT(isConnected());
  IDLE_ASSERT(used().dependent_users_.contains_unsafe(*this));

  IDLE_DETAIL_LOG_DEBUG("Disconnecting usage {} of {}", user().owner(),
                        used().owner());

  Export& used_export = used();
  ContextImpl::from(used_export.owner().root()).call_on_usage_disconnect(*this);

  ServiceImpl::on_usage_disconnect(*this);

  ServiceImpl::dependent_remove(used_export, *this);
  IDLE_ASSERT(!used_export.dependent_users_.contains_unsafe(*this));

  flags_.clear();
  IDLE_ASSERT(flags_.empty());
}

bool Usage::requestRelease() noexcept {
  IDLE_ASSERT(user().owner().root().is_on_event_loop());
  IDLE_ASSERT(isConnected());

  if (flags_.contains(Flags::ReleaseRequested)) {
    return true;
  }

  // It is possible here that the usage gets invalidated
  // on calling usage::on_release already, thus we store the target and source.
  Export& used = this->used();
#if IDLE_DETAIL_HAS_LOG_LEVEL <= IDLE_DETAIL_LOG_LEVEL_DEBUG
  Import& user = this->user();
#endif

  // It is possible that the release is requested
  // and the Usage destroyed instantly.
  flags_.insert(Flags::ReleaseRequested);

  ReleaseReply const response = onReleaseRequest(used);

  if (response != ReleaseReply::Never) {
    IDLE_DETAIL_LOG_DEBUG("{} {} usage of {}.", user,
                          (response == ReleaseReply::Now)
                              ? "immediate released"
                              : "asynchronously releases",
                          used);

    return true;
  } else {
    IDLE_DETAIL_LOG_DEBUG(
        "{} of {} denied releasing usage of {} (response: {}).", user,
        user.owner(), used, response);

    // Weak usages must guarantee to be released on request, otherwise the
    // the system has an unresolvable cyclic dependency.
    IDLE_ASSERT(!isWeak() &&
                "A weak usage must always be released on request!");

    flags_.erase(Flags::ReleaseRequested);
    return false;
  }
}

ReleaseReply Usage::onReleaseRequest(Export& exp) noexcept {
  IDLE_ASSERT(user().owner().root().is_on_event_loop());
  IDLE_ASSERT(isConnected());

  (void)exp;
  return ReleaseReply::Never;
}

void Usage::onUsedDestroy(Export& exp) {
  IDLE_ASSERT(user().owner().root().is_on_event_loop());
  IDLE_ASSERT(used() == exp);
}

void Usage::onUsedStarted(Export& exp) {
  IDLE_ASSERT(user().owner().root().is_on_event_loop());
  IDLE_ASSERT(exp == used());
  IDLE_ASSERT(user().isLocked());
  IDLE_ASSERT(exp.owner().state().isRunning());
}

void Usage::print_usage_details(std::ostream& os) const {
  print(os, FMT_STRING("{}: '{}'/'{}' -> '{}'/'{}'"), name(), used().owner(),
        used(), user().owner(), user());
}

void Usage::print_usage_name(std::ostream& os) const {
  detail::print_class_type(os, *this);
}

bool Export::isAvailable() const noexcept {
  return owner().state().isRunning();
}

bool Interface::operator>(Interface const& other) const noexcept {
  IDLE_ASSERT(type() == other.type());

  Service const& my_owner = owner();
  Service const& other_owner = other.owner();

  IDLE_ASSERT(my_owner.root() == other_owner.root());

  return (!my_owner.state().isDefaultCreated()) >
         (!other_owner.state().isDefaultCreated());
}

Range<PublishedList::iterator> Interface::publishedNeighbors() noexcept {
  return {PublishedList::iterator(*this), {}};
}

Range<PublishedList::const_iterator>
Interface::publishedNeighbors() const noexcept {
  return {PublishedList::const_iterator(*this), {}};
}

void Interface::partName(std::ostream& os) const {
  os << type();
}
} // namespace idle
