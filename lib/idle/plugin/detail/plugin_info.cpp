
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

#include <limits>
#include <LIEF/Abstract/Binary.hpp>
#include <LIEF/Abstract/Parser.hpp>
#include <idle/core/declare.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/util/enum_map.hpp>
#include <idle/core/util/flat_set.hpp>
#include <idle/plugin/detail/plugin_info.hpp>
#include <idle/plugin/detail/shared_library.hpp>

namespace idle {
namespace detail {
static std::vector<std::string>
all_exports_in_section(LIEF::Binary const& bin, std::string const& name) {
  auto const all_sections = bin.sections();

  for (LIEF::Section const& section : all_sections) {
    if (section.name() == name) {
      auto const min = (&section)->virtual_address();
      auto const max = min + (&section)->size();

      IDLE_DETAIL_LOG_TRACE("Section '{}' found (min: {}, max: {}, size: {},"
                            "virtual_address: {}, offset: {}): {}",
                            name, min, max, section.size(),
                            section.virtual_address(), section.offset(),
                            section);

      std::vector<std::string> exports;
      FlatSet<std::uint64_t> addresses;

      for (auto const& fn : bin.exported_functions()) {
        auto const address = fn.address();

        if ((min <= address) && (address < max)) {
          // LIEF under linux yields duplicated addresses?
          if (addresses.find(address) == addresses.end()) {
            exports.push_back(fn.name());
            addresses.insert(address);
          }
        }
      }

      return exports;
    }
  }

  return {};
}

static std::vector<std::string>
all_imported_libraries(LIEF::Binary const& bin) {
  std::vector<std::string> all = bin.imported_libraries();
  for (std::string& imported : all) {
    shared_library::normalize_name(imported);
  }
  return all;
}

static std::vector<std::string> all_exported_symbols(LIEF::Binary const& bin) {
  std::vector<std::string> symbols;
  auto const exported_functions = bin.exported_functions();

  symbols.reserve(exported_functions.size());

  for (auto const& fn : exported_functions) {
    symbols.push_back(fn.name());
  }

  return symbols;
}

plugin_info plugin_info::from(std::string const& path) {
  IDLE_DETAIL_LOG_DEBUG(
      "Reading shared library info of '{}' (services in section '{}')", path,
      IDLE_DETAIL_EXPORT_SECTION_NAME_STR);

  auto const bin = LIEF::Parser::parse(path);

  IDLE_DETAIL_LOG_DEBUG("Parsed: {} as '{}'", bin->name(),
                        enum_name(enumerate<LIEF::EXE_FORMATS>(),
                                  bin->format()));

  plugin_info info;
  info.exported_services_creators = all_exports_in_section(
      *bin, IDLE_DETAIL_EXPORT_SECTION_NAME_STR);
  info.exported_symbols = all_exported_symbols(*bin);
  info.imported_libraries = all_imported_libraries(*bin);

#if IDLE_DETAIL_HAS_LOG_LEVEL <= IDLE_DETAIL_LOG_LEVEL_TRACE
  if (!info.exported_services_creators.empty()) {
    for (std::string const& str : info.exported_services_creators) {
      IDLE_DETAIL_LOG_TRACE("- exported service: {}", str);
    }
  } else {
    IDLE_DETAIL_LOG_TRACE("No service listed as export");
  }
  if (!info.imported_libraries.empty()) {
    for (std::string const& str : info.imported_libraries) {
      IDLE_DETAIL_LOG_TRACE("- imported library: {}", str);
    }
  } else {
    IDLE_DETAIL_LOG_TRACE("No library listed as import");
  }

#endif // IDLE_DETAIL_HAS_CORE_TRACE

  return info;
}
} // namespace detail
} // namespace idle
