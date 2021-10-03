
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

#ifndef IDLE_CORE_PARTS_SCENE_DEPENDENCIES_HPP_INCLUDED
#define IDLE_CORE_PARTS_SCENE_DEPENDENCIES_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/casting.hpp>
#include <idle/core/fwd.hpp>
#include <idle/core/parts/dyn_dependencies.hpp>
#include <idle/core/parts/storage/vector_storage.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/bitset.hpp>
#include <idle/core/util/deref.hpp>
#include <idle/core/util/executor_facade.hpp>

namespace idle {
/// Implements a DynDependencies class that is mutated and accessible from a
/// specified Executor, that needs to implement the ExecutorFacade concept.
template <typename Dependency, typename Executor,
          typename Storage = VectorStorage<Dependency>>
class SceneDependencies final : public DynDependenciesBase, public Storage {
public:
  explicit SceneDependencies(Service& owner, Executor& executor)
    : DynDependenciesBase(owner)
    , owner_(owner)
    , executor_(executor) {}

  Service& owner() noexcept override {
    return owner_;
  }
  Service const& owner() const noexcept override {
    return owner_;
  }

  auto& executor() noexcept {
    return deref(executor_);
  }

protected:
  void doInsert(Interface& dep, StorageUpdate type) noexcept override {
    Storage::onInsert(cast<Dependency>(dep), type);
  }
  void doErase(Interface& dep, StorageUpdate type) noexcept override {
    Storage::onErase(cast<Dependency>(dep), type);
  }

  ReleaseReply onReleaseRequest(Interface& dep) noexcept override {
    IDLE_ASSERT(this->owner().root().is_on_event_loop());

    executor().post(
        [this, dep = std::addressof(cast<Dependency>(dep))]() mutable {
          delDependency(*dep);

          owner_.root().event_loop().post(
              wrap(*this, [dep](Use<SceneDependencies> me) mutable {
                me->doReleaseUsage(*dep);
              }));
        });

    return ReleaseReply::Later;
  }

  UseReply onUseOffer(Interface& dep) noexcept override {
    IDLE_ASSERT(this->owner().root().is_on_event_loop());

    if (dep.owner().state().isRunning()) {
      executor().post([this, dep = std::addressof(cast<Dependency>(dep))] {
        this->addDependency(*dep);
      });
      return UseReply::UseLater;
    } else {
      return UseReply::UseLaterAndNotify;
    }
  }

  BitSet<DependenciesFlags> onInspect(Interface&) noexcept override {
    return {DependenciesFlags::Weak};
  }

  Interface::Id type() const noexcept override {
    return Dependency::id();
  }

  void onImportLock() noexcept override {
    IDLE_ASSERT(owner().root().is_on_event_loop());
    DynDependenciesBase::onImportLock();
    Storage::onLock(*this);
  }

  void onImportUnlock() noexcept override {
    IDLE_ASSERT(owner().root().is_on_event_loop());
    Storage::onUnlock(*this);
    DynDependenciesBase::onImportUnlock();
  }

private:
  void addDependency(Dependency& dep) noexcept {
    IDLE_ASSERT(executor().isThisThread());
    Storage::onInsert(dep, StorageUpdate::Manual);
  }

  void delDependency(Dependency& dep) noexcept {
    IDLE_ASSERT(executor().isThisThread());
    Storage::onErase(dep, StorageUpdate::Manual);
  }

  Service& owner_;
  Executor& executor_;

  IDLE_PART
};
} // namespace idle

#endif // IDLE_CORE_PARTS_SCENE_DEPENDENCIES_HPP_INCLUDED
