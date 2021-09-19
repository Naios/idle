
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

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <idle/core/api.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/platform.hpp>
#include <idle/plugin/detail/shared_library.hpp>

#ifdef IDLE_PLATFORM_WINDOWS
#  include <Psapi.h>
#  include <Windows.h>
#elif defined(IDLE_PLATFORM_MACOS)
#  include <dlfcn.h>
#  include <mach-o/dyld.h>
#else
#  include <dlfcn.h>
#  include <link.h>
#endif

namespace idle {
namespace detail {
namespace shared_library {
optional<handle_t> load(char const* path, boost::dll::fs::error_code& ec) {
#ifdef IDLE_PLATFORM_WINDOWS
  auto handle = LoadLibraryA(path);
  if (!handle) {
    boost::winapi::DWORD_ const errc = boost::winapi::GetLastError();
    ec = boost::dll::fs::error_code(static_cast<int>(errc),
                                    boost::dll::fs::system_category());
    return nullopt;
  }

#else
  auto handle = dlopen(path, RTLD_LAZY);
#endif
  if (handle) {
    return handle;
  } else {
    return nullopt;
  }
}

void* lookup(handle_t handle, char const* name) {
#ifdef IDLE_PLATFORM_WINDOWS
  return reinterpret_cast<void*>(GetProcAddress(handle, name));
#else
  return dlsym(handle, name);
#endif
}

bool unload(handle_t handle) {
#ifdef IDLE_PLATFORM_WINDOWS
  return FreeLibrary(handle) != 0;
#else
  return dlclose(handle) == 0;
#endif
}

// Return the name without its location
static std::string get_module_base_name(char const* module_path) {
  boost::filesystem::path const p(module_path);
  std::string name = p.filename().generic_string();
  normalize_name(name);
  return name;
}

unordered_set<std::string> currently_loaded() {
  unordered_set<std::string> loaded;

#ifdef IDLE_PLATFORM_WINDOWS
  // https://docs.microsoft.com/en-us/windows/win32/psapi/enumerating-all-modules-for-a-process
  HMODULE modules[1024];
  HANDLE const process = GetCurrentProcess();
  DWORD size;

  if (EnumProcessModules(process, modules, sizeof(modules), &size)) {
    for (unsigned i = 0; i < (size / sizeof(HMODULE)); i++) {
      TCHAR module_path[MAX_PATH];

      if (GetModuleFileNameEx(process, modules[i], module_path,
                              sizeof(module_path) / sizeof(TCHAR))) {

        loaded.insert(get_module_base_name(module_path));
      }
    }
  }
#elif defined(IDLE_PLATFORM_MACOS)
  std::uint32_t const count = _dyld_image_count();
  for (std::uint32_t i = 0; i < count; ++i) {
    char const* const module_path = _dyld_get_image_name(i);

    loaded.insert(get_module_base_name(module_path));
  }
#else
  using callback_t = int (*)(struct dl_phdr_info * info, size_t size,
                             void* data);

  dl_iterate_phdr(static_cast<callback_t>([](struct dl_phdr_info* info,
                                             size_t size, void* data) -> int {
                    static_cast<unordered_set<std::string>*>(data)->insert(
                        std::string(info->dlpi_name));
                    return 0;
                  }),
                  &loaded);
#endif

  return loaded;
}

void normalize_name(std::string& path) {
#ifdef IDLE_PLATFORM_WINDOWS
  boost::algorithm::to_lower(path);
#endif
}
} // namespace shared_library
} // namespace detail
} // namespace idle
