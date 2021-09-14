
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

#ifndef IDLE_PLUGIN_PLUGIN_HPP_INCLUDED
#define IDLE_PLUGIN_PLUGIN_HPP_INCLUDED

#include <iosfwd>
#include <string>
#include <vector>
#include <idle/core/api.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/locality.hpp>
#include <idle/core/parts/collection.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/core/views/static_casting.hpp>
#include <idle/plugin/detail/symbol_traits.hpp>

namespace idle {
class Plugin;
class PluginSource;
class Bundle;

/// Represents one instantiation of all services a specific bundle contains
class IDLE_API(idle) Bundle : public Service, protected Container {
  friend class BundleImpl;

  explicit Bundle(Inheritance parent)
    : Service(std::move(parent))
    , Container(*static_cast<Service*>(this)) {}

public:
  using Service::refCounter;
  using Service::weakRefCounter;
  using Service::operator==;
  using Container::children;

  Plugin& parent() noexcept;
  Plugin const& parent() const noexcept;

  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os,
                                                 Bundle const& obj);

  IDLE_SERVICE
};

/// Represents a context of a loaded plugin (shared library),
/// the library will be kept loaded (running) as long as this context exists.
///
/// Symbols and data contained in the associated plugin can be pinned to this
/// reference_counted object to prevent early unloading:
/// ```cpp
/// ref<decltype(data)> data_ref = this_locality().pin(data);
/// ```
class IDLE_API(idle) PluginContext : public ReferenceCounted, public Locality {
  friend class PluginContextImpl;
  explicit PluginContext() = default;

public:
  Plugin& operator*() const noexcept;
  Plugin* operator->() const noexcept;
};

/// Represents a shared library that is loaded while service is running
///
/// This service automatically depends on plugins that are specified
/// in the import section of the underlying shared library.
class IDLE_API(idle) Plugin : public Service,
                              public Interface,
                              protected Collection {
  friend class Bundle;

protected:
  explicit Plugin(Inheritance parent)
    : Service(std::move(parent))
    , Interface(*static_cast<Service*>(this))
    , Collection(*static_cast<Service*>(this)) {}

public:
  using Generation = std::size_t;

  using Interface::classof;
  using Service::refCounter;
  using Service::weakRefCounter;
  using Service::operator==;
  using Service::operator!=;
  using Service::epoch;
  using Service::name;

  /// Instantiates all services exported by this plugin and returns
  /// a new bundle which references all those services.
  Ref<Bundle> createBundle();

  /// Returns all names of functions that are exported by this shared library.
  ///
  /// \param undecorate Whether the function names shall be undecorated.
  std::vector<std::string> const&
  exportedFunctions(bool undecorate = false) const;

  /// Returns a function snapshot to the given function name.
  ///
  /// \param name       The name of the function.
  /// \param undecorate Whether the function names shall be undecorated.
  ///
  /// \returns A symbol which contains a valid function if the
  ///          symbol was found, otherwise the returned symbol
  ///          will be empty.
  template <typename Signature>
  NullableRef<Signature> findSymbol(char const* name, bool undecorate = false) {
    if (void* ptr = symbolPtrOf(name, undecorate)) {
      return {detail::symbol_trait<Signature>::symbol_cast(ptr),
              this->refCounter()};
    } else {
      return {};
    }
  }

  template <typename Signature>
  NullableRef<Signature> findAlias(char const* name) {
    if (void* ptr = aliasPtrOf(name)) {
      return {detail::symbol_trait<Signature>::alias_cast(ptr),
              this->refCounter()};
    } else {
      return {};
    }
  }

  StringView path() const noexcept;
  StringView cachePath() const noexcept;
  StringView location() const noexcept;

  PluginSource& parent() noexcept;
  PluginSource const& parent() const noexcept;

  auto bundles() noexcept {
    return Collection::children() | views::static_casting<Bundle>();
  }
  auto bundles() const noexcept {
    return Collection::children() | views::static_casting<Bundle>();
  }

  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os,
                                                 Plugin const& obj);

private:
  void* symbolPtrOf(char const* name, bool undecorate = false) const;
  void* aliasPtrOf(char const* name) const;

  IDLE_SERVICE
};

class IDLE_API(idle) PluginSource : public Implements<Collection> {
  friend class Plugin;
  friend class PluginSourceImpl;
  using Implements<Collection>::Implements;

public:
  /// Returns the plugin from the given path
  ///
  /// \attention Needs to be called from the event loop.
  Ref<Plugin> load(std::string path, optional<std::string> cache_path = {},
                   Plugin::Generation generation = {});

  /// Prevent sideloading of the given module name
  void banSideload(std::string module_name);
  /// Remove the prevention of sideloading of the given module name
  void unbanSideload(std::string const& module_name);

  auto plugins() noexcept {
    return static_cast<Collection*>(this)->children() |
           views::static_casting<Plugin>();
  }
  auto plugins() const noexcept {
    return static_cast<Collection const*>(this)->children() |
           views::static_casting<Plugin>();
  }

  static Ref<PluginSource> create(Inheritance parent);

  IDLE_SERVICE
};
} // namespace idle

#endif // IDLE_PLUGIN_PLUGIN_HPP_INCLUDED
