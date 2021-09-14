
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

#ifndef IDLE_PLUGIN_DETAIL_PLUGIN_PLUGIN_IMPL_HPP_INCLUDED
#define IDLE_PLUGIN_DETAIL_PLUGIN_PLUGIN_IMPL_HPP_INCLUDED

#include <chrono>
#include <memory>
#include <idle/core/dep/optional.hpp>
#include <idle/core/detail/unordered_map.hpp>
#include <idle/core/detail/unordered_set.hpp>
#include <idle/core/parts/component.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/parts/singleton.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/io_context.hpp>
#include <idle/plugin/detail/plugin_info.hpp>
#include <idle/plugin/detail/shared_library.hpp>
#include <idle/plugin/plugin.hpp>
#include <idle/service/timer.hpp>

namespace idle {
struct PluginPaths {
  std::string path;
  optional<std::string> cache_path;
  optional<std::string> cache_debug_db_path;

  std::string const& library_location() const noexcept {
    if (cache_path) {
      return *cache_path;
    } else {
      return path;
    }
  }
};

class PluginSourceImpl final : public PluginSource,
                               public Upcastable<PluginSourceImpl> {
public:
  using PluginSource::PluginSource;

  Service& owner() noexcept override {
    return *this;
  }
  Service const& owner() const noexcept override {
    return *this;
  }

  continuable<> onStart() override;
  continuable<> onStop() override;

  Ref<Plugin> load_impl(PluginPaths paths, Plugin::Generation generation);

  /// Returns true if the module could be sideloaded or is not needed
  bool sideloadModuleIfNeeded(std::string const& name);

  void banSideloadImpl(std::string module_name) {
    IDLE_ASSERT(root().is_on_event_loop());
    sideload_ban_.emplace(std::move(module_name));
  }
  void unbanSideloadImpl(std::string const& module_name) {
    IDLE_ASSERT(root().is_on_event_loop());
    sideload_ban_.erase(std::move(module_name));
  }

  auto wait_shortly() {
    return timer_->waitFor(std::chrono::milliseconds(100));
  }

  IOContext& thread_pool() noexcept {
    return *io_context_;
  }

  detail::unordered_set<std::string> const& loaded_modules() const noexcept {
    return loaded_modules_;
  }

private:
  Component<Timer> timer_{*this};
  Dependency<IOContext> io_context_{*this};

  detail::unordered_set<std::string> loaded_modules_;
  detail::unordered_map<std::string, detail::shared_library::handle_t>
      sideloaded_modules_;
  detail::unordered_set<std::string> sideload_ban_;
};

class PluginDependency final : public DependencyBase {
public:
  PluginDependency(Service& owner, std::string id) noexcept
    : DependencyBase(owner)
    , id_(std::move(id)) {}

  PluginDependency(PluginDependency&&) = delete;
  PluginDependency(PluginDependency const&) = delete;
  PluginDependency& operator=(PluginDependency&&) = delete;
  PluginDependency& operator=(PluginDependency const&) = delete;

  Interface::Id type() const noexcept override {
    return id_;
  }

private:
  Interface::Id id_;
};

class PluginRegistry : public Collection {
public:
  using Collection::Collection;

  IDLE_PART
};

class PluginImpl final : public Extends<Plugin, PluginRegistry>,
                         public Upcastable<PluginImpl> {
  friend class BundleImpl;

public:
  explicit PluginImpl(Inheritance inh, PluginPaths path, Generation generation);
  virtual ~PluginImpl();

  void onDestroy() override;

  continuable<> onStart() override;
  continuable<> onStop() override;

  Service& owner() noexcept override {
    return *this;
  }
  Service const& owner() const noexcept override {
    return *this;
  }

  PluginSourceImpl& parent() noexcept {
    return PluginSourceImpl::from(Plugin::parent());
  }
  PluginSourceImpl const& parent() const noexcept {
    return PluginSourceImpl::from(Plugin::parent());
  }

  std::vector<std::string> const&
  exported_functions_impl(bool undecorate) const;

  std::vector<std::string> const& exported_services_symbols() const noexcept {
    return info_.exported_services_creators;
  }

  void* symbol_ptr_of_impl(char const* name, bool undecorate = false) const;

  void* alias_ptr_of_impl(char const* name) const;

  StringView path_impl() const {
    return paths_.path;
  }

  StringView cache_path_impl() const {
    if (paths_.cache_path) {
      return {*paths_.cache_path};
    } else {
      return {};
    }
  }

  StringView location_impl() const {
    return paths_.library_location();
  }

  Collection& bundleCollection() noexcept {
    return static_cast<Plugin&>(*this);
  }

  void on_context_released() noexcept;

  bool operator>(Interface const& other) const noexcept override;

protected:
  void name(std::ostream& os) const override;

  Id type() const noexcept override {
    return id_;
  }

private:
  void inject(PluginContext& owner) noexcept;

  Id const id_;

  Singleton singleton_;

  PluginPaths paths_;
  Generation generation_;
  detail::shared_library::handle_t handle_;
  detail::plugin_info info_;

  PluginDependency* dependencies_;
  std::size_t dependencies_size_;

  RefCounter plugin_context_;
  promise<> promise_;
};

class PluginContextImpl final : public PluginContext,
                                public Upcastable<PluginContextImpl> {
public:
  explicit PluginContextImpl(Ref<PluginImpl> plugin) noexcept;
  ~PluginContextImpl() override;

  PluginImpl& get_plugin() const noexcept {
    return *plugin_;
  }

  PluginRegistry& anchor() noexcept override {
    return *plugin_;
  }
  PluginRegistry const& anchor() const noexcept override {
    return *plugin_;
  }

private:
  Ref<PluginImpl> plugin_;
};

class BundleImpl final : public Bundle, public Upcastable<BundleImpl> {
public:
  explicit BundleImpl(Inheritance parent)
    : Bundle(std::move(parent)) {}

  continuable<> onStart() override;
  continuable<> onStop() override;

  Service& owner() noexcept override {
    return *this;
  }
  Service const& owner() const noexcept override {
    return *this;
  }

  PluginImpl& parent() noexcept;
  PluginImpl const& parent() const noexcept;

  static Ref<Bundle> create_from(PluginImpl& plugin);

private:
  void create_children();
  void destroy_children();
};
} // namespace idle

#endif // IDLE_PLUGIN_DETAIL_PLUGIN_PLUGIN_IMPL_HPP_INCLUDED
