
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

#include <vector>
#include <idle/core/dep/optional.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/plugin/detail/demangle.hpp>
#include <llvm/Demangle/Demangle.h>

namespace idle {
struct free_deleter {
  void operator()(char* p) const {
    std::free(p);
  }
};

static std::unique_ptr<char, free_deleter> alloc_buffer(std::size_t* n) {
  *n = 1024;
  auto ptr = static_cast<char*>(std::malloc(*n));
  return std::unique_ptr<char, free_deleter>(ptr);
}

static optional<std::string> demangle_select(mangeling selected,
                                             char const* name, char* buffer,
                                             std::size_t* n) {
  int status = 0;

  if (selected == mangeling::itanium) {
    llvm::itaniumDemangle(name, buffer, n, &status);
  } else {
    IDLE_ASSERT(selected == mangeling::microsoft);
    llvm::microsoftDemangle(name, buffer, n, &status);
  }

  if (status) {
    return nullopt;
  } else {
    return std::string(buffer, *n);
  }
}

optional<std::string> demangle(mangeling selected, char const* name) {
  std::size_t n = 0;
  auto buffer = alloc_buffer(&n);
  return demangle_select(selected, name, buffer.get(), &n);
}

std::unordered_map<std::string, std::string>
demangle_all(mangeling selected, std::vector<std::string> names) {
  std::size_t n = 0;
  auto buffer = alloc_buffer(&n);

  std::unordered_map<std::string, std::string> demangled_names;
  for (auto&& name : names) {
    if (auto dm = demangle_select(selected, name.c_str(), buffer.get(), &n)) {
      demangled_names.insert(std::make_pair(std::move(name), std::move(*dm)));
    }
  }

  return demangled_names;
}
} // namespace idle
