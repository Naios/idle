
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

#include <ostream>
#include <vector>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <idle/core/context.hpp>
#include <idle/core/declare.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/platform.hpp>
#include <idle/core/service.hpp>
#include <idle/plugin/detail/plugin/plugin_impl.hpp>
#include <idle/plugin/detail/shared_library.hpp>

namespace idle {
static optional<char const*> platform_debug_symbol_db_extension() {
#ifdef _WIN32
  return "pdb";
#else
  return nullopt;
#endif
}

continuable<> PluginSourceImpl::onStart() {
  return async([this] {
    IDLE_ASSERT(loaded_modules_.empty());
    IDLE_ASSERT(sideload_ban_.empty());
    IDLE_ASSERT(sideloaded_modules_.empty());

    // Store the currently loaded modules in order to remove them
    // later from the dependency list of plugins.
    loaded_modules_ = detail::shared_library::currently_loaded();
  });
}

continuable<> PluginSourceImpl::onStop() {
  return async([=] {
    loaded_modules_.clear();
    sideload_ban_.clear();

    for (auto&& entry : sideloaded_modules_) {
      detail::shared_library::unload(entry.second);
    }

    sideloaded_modules_.clear();

    for (Plugin& plugin : plugins()) {
      plugin.destroy();
    }
  });
}

Ref<Plugin> PluginSourceImpl::load_impl(PluginPaths paths,
                                        Plugin::Generation generation) {

  IDLE_ASSERT(root().is_on_event_loop());
  IDLE_ASSERT(state().isRunning());

  IDLE_DETAIL_LOG_DEBUG("Loading plugin {}", paths.path);

  IDLE_ASSERT(!paths.path.empty());
  IDLE_ASSERT(boost::filesystem::exists(paths.path));

  if (paths.cache_path) {
    IDLE_DETAIL_LOG_DEBUG("Copying plugin '{}' to cache path '{}'", paths.path,
                          *paths.cache_path);

    boost::filesystem::copy_file(
        paths.path, *paths.cache_path,
        boost::filesystem::copy_options::overwrite_existing);

    if (auto ext = platform_debug_symbol_db_extension()) {
      boost::filesystem::path debug_symbol_db(paths.path);
      debug_symbol_db.replace_extension(*ext);

      if (exists(debug_symbol_db)) {
        boost::filesystem::path target(*paths.cache_path);
        target.replace_extension(*ext);

        copy_file(debug_symbol_db, target,
                  boost::filesystem::copy_options::overwrite_existing);

        paths.cache_debug_db_path = target.generic_string();
      }
    }
  }

  Ref<PluginImpl> ptr = spawn<PluginImpl>(Inheritance(
                                              *static_cast<Collection*>(this)),
                                          std::move(paths), generation);
  ptr->init();
  return std::move(ptr).cast<Plugin>();
}

bool PluginSourceImpl::sideloadModuleIfNeeded(std::string const& name) {
  IDLE_ASSERT(root().is_on_event_loop());

  IDLE_DETAIL_LOG_TRACE("Checking sideload of '{}'...", name);

  if (sideload_ban_.find(name) != sideload_ban_.end()) {
    IDLE_DETAIL_LOG_TRACE("'{}' is banned from sideload", name);
    return false;
  }

  if (sideloaded_modules_.find(name) != sideloaded_modules_.end()) {
    IDLE_DETAIL_LOG_TRACE("'{}' should be loaded already", name);
    return true;
  }

  boost::system::error_code ec;
  if (auto handle = detail::shared_library::load(name.c_str(), ec)) {
    IDLE_DETAIL_LOG_DEBUG("Sideloaded shared library {}", name);
    sideloaded_modules_.insert(std::make_pair(name, *handle));
    return true;
  }

  IDLE_DETAIL_LOG_TRACE("Failed to sideload shared library {}", name);
  return false;
}

static std::string plugin_id_of_name(StringView name) {
  return format(FMT_STRING("idle::Plugin<{}>"), name);
}

static std::string plugin_id_of_path(boost::filesystem::path const& path) {
  std::string name = path.filename().generic_string();
  detail::shared_library::normalize_name(name);
  return plugin_id_of_name(name);
}

PluginContextImpl::PluginContextImpl(Ref<PluginImpl> plugin) noexcept
  : plugin_(std::move(plugin)) {}

PluginContextImpl::~PluginContextImpl() {
  Plugin& owner = *plugin_;
  owner.root().event_loop().dispatch([plugin_ = std::move(plugin_)] {
    plugin_->on_context_released();
  });
}

PluginImpl::PluginImpl(Inheritance inh, PluginPaths path, Generation generation)
  : Extends<Plugin, PluginRegistry>(std::move(inh))
  , id_(Id(plugin_id_of_path(path.path)))
  , singleton_(static_cast<Plugin&>(*this))
  , paths_(std::move(path))
  , generation_(generation)
  , handle_{}
  , info_(detail::plugin_info::from(paths_.library_location())) {

  IDLE_ASSERT(root().is_on_event_loop());

  // Get the modules loaded when the plugin_source was started.
  // This makes it possible to filter out dependencies that are required
  // by default through the application itself or were loaded in advance.
  auto const& loaded = this->parent().loaded_modules();

  std::vector<StringView> required;
  for (std::string const& imported : info_.imported_libraries) {
    if (loaded.find(imported) != loaded.end()) {
      // If the library was loaded when the PluginSource started we are fine
      continue;
    }

    if (this->parent().sideloadModuleIfNeeded(imported)) {
      // Attempt to dynamically load the shared library
      // This is required for system libraries or others in the path
      continue;
    }

    IDLE_DETAIL_LOG_TRACE("Shared library '{}' added  '{}' as dependency",
                          paths_.path, imported);

    required.emplace_back(imported);
  }

  dependencies_size_ = required.size();
  std::allocator<PluginDependency> alloc;
  dependencies_ = alloc.allocate(dependencies_size_);

  std::size_t i = 0;
  for (; i < dependencies_size_; ++i) {
    PluginDependency& current = dependencies_[i];
    new (&current) PluginDependency(*this, plugin_id_of_name(required[i]));
  }
}

PluginImpl::~PluginImpl() {
  for (std::size_t i = 0; i < dependencies_size_; ++i) {
    dependencies_[i].~PluginDependency();
  }

  std::allocator<PluginDependency> alloc;
  alloc.deallocate(dependencies_, dependencies_size_);
}

void PluginImpl::onDestroy() {
  boost::system::error_code ec;

  if (paths_.cache_path) {
    boost::filesystem::remove(*paths_.cache_path, ec);

    if (ec) {
      IDLE_DETAIL_LOG_ERROR(
          "Failed to delete cached shared library at '{}' ('{}').",
          *paths_.cache_path, ec.message());
    }
  }
  if (paths_.cache_debug_db_path) {
    boost::filesystem::remove(*paths_.cache_debug_db_path, ec);

    if (ec) {
      IDLE_DETAIL_LOG_ERROR(
          "Failed to delete cached debug information at '{}' ('{}').",
          *paths_.cache_debug_db_path, ec.message());
    }
  }

  Plugin::onDestroy();
}

#ifndef NDEBUG
static bool isOnlyRunning(Interface& me) {
  for (Interface& inter : me.registry().interfaces()) {
    if (inter == me) {
      continue;
    }
    if (!inter.owner().state().isStopped()) {
      IDLE_DETAIL_LOG_ERROR(
          "Failed checking uniqueness of plugin {} because the other plugin {} "
          "is not in a stopped state already!",
          me, inter);
      return false;
    }
  }
  return true;
}
#endif

continuable<> PluginImpl::onStart() {
  return async_on(
      [this]() mutable {
        IDLE_ASSERT(root().is_on_event_loop());
        IDLE_DETAIL_LOG_DEBUG("Loading shared library {}", location_impl());

        IDLE_ASSERT(isOnlyRunning(*this));

        boost::system::error_code ec;
        if (auto handle = detail::shared_library::load(
                paths_.library_location().c_str(), ec)) {
          handle_ = *handle;
        } else {
          IDLE_DETAIL_LOG_ERROR("Failed to load the plugin '{}' natively, real "
                                "location: '{}'! ({})",
                                paths_.path, paths_.library_location(),
                                (ec.failed()) ? ec.message() : "no error code");

          throw boost::system::error_code(ec);
        }

        Ref<PluginContextImpl> current = make_ref<PluginContextImpl>(
            refOf(*this));
        inject(*current);
        plugin_context_ = RefCounter(std::move(current));
      },
      root().event_loop().through_post());
}

continuable<> PluginImpl::onStop() {
  return make_continuable<void>([this](promise<>&& promise) mutable {
           IDLE_ASSERT(root().is_on_event_loop());

           IDLE_ASSERT(isOnlyRunning(*this));

           promise_ = std::move(promise);

           IDLE_DETAIL_LOG_DEBUG(
               "Plugin context of {} has {} remaining strong references", *this,
               plugin_context_.count_unsafe() - 1U);

           plugin_context_.reset();
           PluginRegistry::clear();
         })
      // .then(parent().wait_shortly())
      .then(
          [this]() mutable {
            IDLE_DETAIL_LOG_DEBUG("Unloading shared library {}",
                                  location_impl());

            IDLE_CHECK(detail::shared_library::unload(handle_));

            // TODO Maybe check if PDB is (exclusively) writeable to ensure the
            // debugger has released it which makes it possible to delete the
            // PDB on destroy right away.
            // Also we could check for open file handles.
            // Because we use the PDB altpath the PDB handle by MSVC should be
            // released soon after unload.

            // TODO Additionally we could introduce another artificial parent
            // that handles the file caching so the lifetime of this service
            // is not dependent on whether the cache files were deleted or not.
          },
          root().event_loop().through_post());
}

std::vector<std::string> const&
PluginImpl::exported_functions_impl(bool undecorate) const {
  // TODO Add demangeling
  // TODO Only return functions
  (void)undecorate;
  return info_.exported_symbols;
}

void* PluginImpl::symbol_ptr_of_impl(char const* name, bool undecorate) const {
  IDLE_ASSERT(handle_);

  // TODO Add demangeling
  (void)undecorate;
  return detail::shared_library::lookup(handle_, name);
}

void* PluginImpl::alias_ptr_of_impl(char const* name) const {
  return symbol_ptr_of_impl(
      fmt::format(FMT_STRING("idle_plugin_alias_{}"), name).c_str(), false);
}

void PluginImpl::inject(PluginContext& owner) noexcept {
  // Inject the plugin_context into the plugin itself
  using set_context_t = void(WeakRef<PluginContext>);
  if (auto set_context = this->findAlias<set_context_t>("set_context")) {

    IDLE_DETAIL_LOG_DEBUG("Injecting context into {} ({})", *this,
                          static_cast<void const*>(&owner));

    auto weak = weakOf(owner);
    IDLE_ASSERT(!weak.expired());
    (*set_context)(std::move(weak));
  } else {
    IDLE_DETAIL_LOG_DEBUG("Didn't find a context injection hook for {}", *this);
  }
}

void PluginImpl::on_context_released() noexcept {
  IDLE_ASSERT(root().is_on_event_loop());
  promise_.set_value();
}

bool PluginImpl::operator>(Interface const& other) const noexcept {
  return generation_ > cast<PluginImpl>(other).generation_;
}

void PluginImpl::name(std::ostream& os) const {
  os << id_;
}

/*
void PluginImpl::printDetails(std::ostream& os) const {
  Plugin::printDetails(os);

  os << ", generation: " << generation_;

  os << ", path: \'" << path() << "\'";

  if (paths_.cache_path) {
    os << ", cached at: \'" << *paths_.cache_path << "\'";
  }
}
*/

continuable<> BundleImpl::onStart() {
  return async([this]() mutable {
    IDLE_ASSERT(root().is_on_event_loop());
    create_children();
  });
}

continuable<> BundleImpl::onStop() {
  return async([this]() mutable {
    IDLE_ASSERT(root().is_on_event_loop());
    destroy_children();
  });
}

PluginImpl& BundleImpl::parent() noexcept {
  return PluginImpl::from(Bundle::parent());
}

PluginImpl const& BundleImpl::parent() const noexcept {
  return PluginImpl::from(Bundle::parent());
}

Ref<Bundle> BundleImpl::create_from(PluginImpl& plugin) {
  auto bundle = spawn<BundleImpl>(plugin.bundleCollection().inherit());

  plugin.root().event_loop().dispatch([bundle] {
    bundle->init();
  });

  return std::move(bundle).cast<Bundle>();
}

void BundleImpl::create_children() {
  IDLE_ASSERT(root().is_on_event_loop());

  auto const& service_creators = parent().exported_services_symbols();
  for (std::string const& creator_name : service_creators) {
    // Retrieve the pointer to the factory function of the service
    void* symbol = parent().symbol_ptr_of_impl(creator_name.c_str(), false);
    IDLE_ASSERT(symbol);

#ifdef IDLE_PLATFORM_WINDOWS
    auto const creator = *static_cast<detail::service_creator_ptr_t*>(symbol);
#else
    auto const creator = reinterpret_cast<detail::service_creator_ptr_t>(
        symbol);
#endif

    // Create the service object and return it from inside the bundle
    // because we aren't allowed to acquire a strong reference here to
    // avoid a cyclic reference since the service references the
    // plugin in a strong way already.
    auto service = creator(inherit());
    IDLE_ASSERT(service);
    service->init();
  }
}

void BundleImpl::destroy_children() {
  IDLE_ASSERT(root().is_on_event_loop());

  // Destroy all children immediately
  while (auto const child = children()) {
    Service& first = child.front();
    first.destroy();
  }
}
} // namespace idle
