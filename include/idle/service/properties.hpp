
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

#ifndef IDLE_SERVICE_PROPERTIES_HPP_INCLUDED
#define IDLE_SERVICE_PROPERTIES_HPP_INCLUDED

#include <string>
#include <type_traits>
#include <idle/core/api.hpp>
#include <idle/core/interface.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/service/art/reflection.hpp>

namespace idle {
class Sink;

class IDLE_API(idle) PropertiesSource {
public:
  virtual ~PropertiesSource();

  virtual void read(std::istream& is, StringView file_name) = 0;
  virtual void write(std::ostream& is, StringView file_name) = 0;

  virtual bool serialize(ConstReflectionPtr ptr, StringView key,
                         Sink& sink) noexcept = 0;
  virtual bool deserialize(ReflectionPtr ptr, StringView key,
                           Sink& sink) const noexcept = 0;

  virtual bool equals(PropertiesSource const& other,
                      StringView key) const noexcept = 0;

  virtual Ref<PropertiesSource> create() const = 0;
  virtual Ref<PropertiesSource> clone() const = 0;
};

class IDLE_API(idle) Properties : public Interface {
public:
  using Generation = std::size_t;

  explicit Properties(Service& owner, Generation generation);

  /// Returns true if the given key has changed from the previous generation.
  virtual bool changed(StringView key) const noexcept = 0;

  /// Stores the structure of the given key into the reflectable object
  virtual bool get(ReflectionPtr out, StringView key) const noexcept = 0;

  /// Stores the values of the reflectable object into the given key
  ///
  /// \attention Needs to be called from the event loop!
  virtual void set(ConstReflectionPtr in, StringView key) noexcept = 0;

  Generation generation() const noexcept {
    return generation_;
  }

  bool operator>(Interface const& other) const noexcept final;

private:
  Generation generation_;

  IDLE_INTERFACE
};

class IDLE_API(idle) ReloadableProperties : public Service {
  friend class ReloadablePropertiesImpl;

  using Service::Service;

public:
  static std::string defaultLocation();

  struct Config {
    std::string path;
    bool write_back{true};
  };

  void setup(Config config);

  static Ref<ReloadableProperties> create(Inheritance inh);

  IDLE_SERVICE
};
} // namespace idle

#endif // IDLE_SERVICE_PROPERTIES_HPP_INCLUDED
