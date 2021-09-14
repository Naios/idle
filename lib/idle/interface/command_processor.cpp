
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

#include <algorithm>
#include <mutex>
#include <utility>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/parts/dependency_set.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/use.hpp>
#include <idle/core/util/functional.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/interface/arguments.hpp>
#include <idle/interface/command.hpp>
#include <idle/interface/command_processor.hpp>
#include <idle/interface/logger.hpp>
#include <tsl/htrie_map.h>

namespace idle {
class DefaultCommandProcessor final : public Implements<CommandProcessor>,
                                      public Subscriber {
  // TODO Upgrade this to a shared mutex in C++17
  using Mutex = std::mutex;
  using ReadLock = std::lock_guard<Mutex>;
  using WriteLock = std::lock_guard<Mutex>;

public:
  explicit DefaultCommandProcessor(Inheritance parent)
    : Implements<CommandProcessor>(std::move(parent))
    , Subscriber({Event::OnCreated, Event::OnDestroy}) {}

  using Implements<CommandProcessor>::Implements;

  void onInit() override {
    command_registry_ = root().find<Command>();

    // Populate all existing commands
    for (Interface& command : command_registry_->interfaces()) {
      Command& current = cast<Command>(command);
      auto const name = current.current_command_name();
      commands_.insert_ks(name.data(), name.size(), current);
    }

    command_registry_->subscriberAdd(*this);
  }

  void onDestroy() override {
    command_registry_->subscriberDel(*this);

    commands_.clear();

    command_registry_.reset();
  }

  void onSubscribedCreated(Interface& subscribed) override {
    WriteLock lock(mutex_);

    Command& current = cast<Command>(subscribed);
    auto const name = current.current_command_name();

    commands_.insert_ks(name.data(), name.size(), current);
  }

  void onSubscribedDestroy(Interface& subscribed) override {
    WriteLock lock(mutex_);

    Command& current = cast<Command>(subscribed);
    auto const name = current.current_command_name();

    commands_.insert_ks(name.data(), name.size(), current);
  }

  continuable<> invoke(Ref<Session> session, std::string args) override {
    return async_on(
        wrap(*this,
             [session = std::move(session),
              args = std::move(args)](auto&& me) mutable -> continuable<> {
               IDLE_ASSERT(me->root().is_on_event_loop());

               if (auto const result = me->longestPrefixMatch(args)) {
                 auto const name = result->current_command_name();
                 auto const prefix_free = StringView(args).substr(name.size());

                 // Acquire the usage regardless whether the command is started
                 me->dependencies_.acquireUsage(*result);

                 if (result->owner().state().isRunning()) {
                   return result->invoke(std::move(session),
                                         Arguments::from(prefix_free));
                 } else {
                   return result->owner().start().then(
                       result->invoke(std::move(session),
                                      Arguments::from(prefix_free)));
                 }
               } else {
                 IDLE_LOG_ERROR(me->log_, "There is no such command as '{}'!",
                                args);

                 return make_ready_continuable();
               }
             }),
        root().event_loop().through_dispatch());
  }

  std::vector<std::string> complete(Session& session,
                                    StringView command) noexcept override {
    ReadLock lock(mutex_);

    // For now complete everything non session agnostic
    (void)session;

    std::vector<std::string> completions;

    auto const range = commands_.equal_prefix_range_ks(command.data(),
                                                       command.size());
    for (auto itr = range.first; itr != range.second; ++itr) {
      completions.push_back(itr.key());
    }

    return completions;
  }

  ReleaseReply onReleaseRequest(Command& dep) noexcept {
    // TODO Make sure that the command is not processed currently
    dependencies_.releaseUsage(dep);
    return ReleaseReply::Later;
  }

  BitSet<DependenciesFlags> onInspect(Command const&) noexcept {
    return {DependenciesFlags::Ignore, DependenciesFlags::Weak};
  }

private:
  /// Returns the command with the longest prefix matched
  Nullable<Command> longestPrefixMatch(StringView command) {
    ReadLock lock(mutex_);

    auto const longest = commands_.longest_prefix_ks(command.data(),
                                                     command.size());
    if (longest != commands_.end()) {
      return longest->get();
    } else {
      return {};
    }
  }

  Mutex mutex_;
  tsl::htrie_map<char, std::reference_wrapper<Command>> commands_;

  Ref<Registry> command_registry_;

  Dependency<Log> log_{*this};

  DynDependencies<Command, DefaultCommandProcessor> dependencies_{
      *this,                                                        //
      IDLE_STATIC_BIND(&DefaultCommandProcessor::onReleaseRequest), //
      IDLE_STATIC_RETURN(UseReply::UseNever),                       //
      IDLE_STATIC_BIND(&DefaultCommandProcessor::onInspect)};

  IDLE_SERVICE
};

Ref<CommandProcessor> CommandProcessor::create(Inheritance parent) {
  return spawn<DefaultCommandProcessor>(std::move(parent));
}
} // namespace idle
