
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

#ifndef IDLE_SERVICE_STORE_HPP_INCLUDED
#define IDLE_SERVICE_STORE_HPP_INCLUDED

#include <chrono>
#include <cstdint>
#include <string>
#include <idle/core/api.hpp>
#include <idle/core/dep/function.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/span.hpp>
#include <idle/core/util/type_name.hpp>
#include <idle/service/art/reflection_tree.hpp>

namespace idle {
class IDLE_API(idle) Store : public Interface {
public:
  /// Implements an opaque id
  class ID {
  public:
    using type = std::uint64_t;

    explicit ID(type value)
      : value_(value) {}
    ID() noexcept = default;
    ID(ID const&) = delete;
    ID(ID&& other)
    noexcept
      : value_(std::exchange(other.value_, {})) {}
    ID& operator=(ID const&) = delete;
    ID& operator=(ID&& other) noexcept {
      value_ = std::exchange(other.value_, {});
      return *this;
    }

    constexpr explicit operator bool() const noexcept {
      return !!value_;
    }
    constexpr type operator*() const noexcept {
      return value_;
    }

  private:
    type value_{};
  };

  using BufferView = Span<char const>;
  using Consumer = function_view<void(BufferView)>;

  using Interface::Interface;

  template <typename Service>
  ID get(Service const&, ReflectionPtr out) {
    return get(StringView(type_name<Service>()), out);
  }
  ID get(StringView key, ReflectionPtr out);
  template <typename Service>
  ID get(Service const&, Consumer callback) {
    return get(StringView(type_name<Service>()), std::move(callback));
  }
  virtual ID get(StringView key, Consumer callback) = 0;

  void update(ID const& id, ConstReflectionPtr in);
  virtual void update(ID const& id, BufferView buffer) = 0;

  void set(ID&& id, ConstReflectionPtr in);
  virtual void set(ID&& id, BufferView buffer) = 0;

  static Ref<Store> create(Inheritance parent);

  IDLE_INTERFACE
};

class IDLE_API(idle) DefaultStore : public Implements<Store> {
  friend class DefaultStoreImpl;
  using Implements<Store>::Implements;

public:
  struct Config {
    std::string path;
  };

  void setup(Config config);

  static std::string defaultLocation();

  static Ref<DefaultStore> create(Inheritance parent);

  IDLE_SERVICE
};
} // namespace idle

#endif // IDLE_SERVICE_STORE_HPP_INCLUDED
