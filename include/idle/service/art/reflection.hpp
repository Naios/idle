
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

#ifndef IDLE_SERVICE_ART_REFLECTION_HPP_INCLUDED
#define IDLE_SERVICE_ART_REFLECTION_HPP_INCLUDED

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <idle/core/api.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/checked.hpp>
#include <idle/core/util/meta.hpp>
#include <idle/core/util/nullable.hpp>
#include <idle/core/util/range.hpp>
#include <idle/core/util/span.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
/// ART - Abstract Reflection Tree
namespace art {
enum class MappedType : std::uint8_t {
#define IDLE_DETAIL_FOR_EACH_MAPPED_TYPE(NAME) NAME,
#include <idle/service/art/reflection.inl>
};

IDLE_API(idle) std::ostream& operator<<(std::ostream& os, MappedType value);

constexpr bool isBool(MappedType type) noexcept {
  return type == MappedType::Bool;
}
constexpr bool isIntegral(MappedType type) noexcept {
  switch (type) {
    case MappedType::Int8:
    case MappedType::Int16:
    case MappedType::Int32:
    case MappedType::Int64:
    case MappedType::UInt8:
    case MappedType::UInt16:
    case MappedType::UInt32:
    case MappedType::UInt64:
      return true;
    default:
      return false;
  }
}
constexpr bool isSigned(MappedType type) noexcept {
  switch (type) {
    case MappedType::Int8:
    case MappedType::Int16:
    case MappedType::Int32:
    case MappedType::Int64:
      return true;
    default:
      return false;
  }
}
constexpr bool isFloating(MappedType type) noexcept {
  switch (type) {
    case MappedType::Float:
    case MappedType::Double:
      return true;
    default:
      return false;
  }
}
constexpr bool isString(MappedType type) noexcept {
  return type == MappedType::String;
}
constexpr bool isTimePoint(MappedType type) noexcept {
  return type == MappedType::TimePoint;
}
constexpr bool isDuration(MappedType type) noexcept {
  return type == MappedType::Duration;
}
constexpr bool isArray(MappedType type) noexcept {
  return type == MappedType::Array;
}
constexpr bool isSet(MappedType type) noexcept {
  return type == MappedType::Set;
}
constexpr bool isObject(MappedType type) noexcept {
  return type == MappedType::Object;
}
constexpr bool isStructured(MappedType type) noexcept {
  switch (type) {
#define IDLE_DETAIL_FOR_EACH_MAPPED_STRUCTURE(NAME)                            \
  case MappedType::NAME:                                                       \
    return true;
#include <idle/service/art/reflection.inl>
    default:
      return false;
  }
}
constexpr bool isPrimitive(MappedType type) noexcept {
  return !isStructured(type);
}

class FieldType;
} // namespace art

class IDLE_API(idle) Reflection {
public:
  Reflection() {}

  virtual Span<art::FieldType const> fields() const noexcept = 0;
  virtual Nullable<Reflection const> super() const noexcept = 0;

  Nullable<art::FieldType const> find(StringView name) const noexcept;
};

/// Represents an abstract reflected object
///
/// \note Use this type to accept an arbitrary reflectable object through
///       a function signature.
class ReflectionPtr {
public:
  using pointer = void*;

  constexpr ReflectionPtr(ReflectionPtr const&) noexcept = default;
  constexpr ReflectionPtr& operator=(ReflectionPtr const&) noexcept = default;

  constexpr ReflectionPtr(pointer object, Reflection const& reflection) noexcept
    : object_(object)
    , reflection_(&reflection) {}

  template <typename T, std::enable_if_t<!std::is_same<
                            ReflectionPtr, unrefcv_t<T>>::value>* = nullptr>
  constexpr /*implicit*/ ReflectionPtr(T& object) noexcept
    : ReflectionPtr(std::addressof(object),
                    reflect(static_cast<unrefcv_t<T> const*>(nullptr))) {}

  constexpr pointer object() const noexcept {
    return object_;
  }
  constexpr Reflection const& reflection() const noexcept {
    return *reflection_;
  }

  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os,
                                                 ReflectionPtr const& ptr);

private:
  pointer object_;
  Reflection const* reflection_;
};
/// \copydoc ReflectionPtr
class ConstReflectionPtr {
public:
  using pointer = void const*;

  constexpr ConstReflectionPtr(ConstReflectionPtr const&) noexcept = default;
  constexpr ConstReflectionPtr&
  operator=(ConstReflectionPtr const&) noexcept = default;

  constexpr ConstReflectionPtr(ReflectionPtr const& other) noexcept
    : object_(other.object())
    , reflection_(&other.reflection()) {}

  constexpr ConstReflectionPtr(pointer object,
                               Reflection const& reflection) noexcept
    : object_(object)
    , reflection_(&reflection) {}

  template <
      typename T,
      std::enable_if_t<
          !std::is_same<ReflectionPtr, unrefcv_t<T>>::value &&
          !std::is_same<ConstReflectionPtr, unrefcv_t<T>>::value>* = nullptr>
  constexpr /*implicit*/ ConstReflectionPtr(T& object) noexcept
    : ConstReflectionPtr(std::addressof(object),
                         reflect(static_cast<unrefcv_t<T> const*>(nullptr))) {}

  constexpr pointer object() const noexcept {
    return object_;
  }
  constexpr Reflection const& reflection() const noexcept {
    return *reflection_;
  }

  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os,
                                                 ConstReflectionPtr const& ptr);

private:
  pointer object_;
  Reflection const* reflection_;
};

namespace art {
class Type {
public:
  explicit constexpr Type(MappedType type) noexcept
    : type_(type) {}

  constexpr MappedType type() const noexcept {
    return type_;
  }

private:
  MappedType type_;
};

class Subtyped : public Type {
public:
  constexpr Subtyped(MappedType type, Type const* subtype) noexcept
    : Type(type)
    , subtype_(subtype) {
    IDLE_ASSERT((subtype != nullptr) || isPrimitive(type));
  }

  bool hasSubtype() const noexcept {
    return subtype_ != nullptr;
  }
  Type const& subtype() const noexcept {
    IDLE_ASSERT(hasSubtype());
    return *subtype_;
  }

private:
  Type const* subtype_;
};

class IDLE_API(idle) PrimitiveType : public Type {
public:
  struct Entry {
    StringView name;
    void const* value{};
  };

  explicit constexpr PrimitiveType(MappedType type, std::size_t size,
                                   Entry const* entries,
                                   std::size_t const* values,
                                   std::size_t const* names) noexcept
    : Type(type)
    , size_(size)
    , entries_(entries)
    , values_(values)
    , names_(names) {}

  constexpr Span<Entry const> entries() const noexcept {
    return {entries_, size_};
  }
  constexpr Span<std::size_t const> values() const noexcept {
    return {values_, size_};
  }
  constexpr Span<std::size_t const> names() const noexcept {
    return {names_, size_};
  }

  /// Returns the corresponding representation of the given name
  ///
  /// \attention This function returns a nullptr if the name was not found!
  void const* value(StringView name) const noexcept;

  /// Returns the corresponding name of the given representation
  ///
  /// \attention This function returns a nullptr if the value was not found!
  StringView name(void const* value) const noexcept;

  constexpr Entry const& operator[](std::size_t size) const noexcept {
    IDLE_ASSERT(size < size_);
    return entries_[size];
  }

private:
  std::size_t size_;
  Entry const* entries_;
  std::size_t const* values_;
  std::size_t const* names_;
};

class FieldType final : public Subtyped {
public:
  FieldType(MappedType type, Type const* subtype, char const* name,
            char const* description, std::size_t offset) noexcept
    : Subtyped(type, subtype)
    , offset_(checked_narrow<std::uint16_t>(offset))
    , name_(name)
    , description_(description) {}

  constexpr char const* name() const noexcept {
    return name_;
  }
  constexpr char const* description() const noexcept {
    return description_;
  }

  constexpr std::size_t offset() const noexcept {
    return offset_;
  }
  void* relocate(void* current) const noexcept {
    return reinterpret_cast<void*>(reinterpret_cast<std::size_t>(current) +
                                   offset());
  }
  void const* relocate(void const* current) const noexcept {
    return reinterpret_cast<void const*>(
        reinterpret_cast<std::size_t>(current) + offset());
  }

private:
  std::uint16_t offset_;
  char const* name_;
  char const* description_;
};

class IDLE_API(idle) ArrayType : public Subtyped {
public:
  explicit constexpr ArrayType(MappedType type, Type const* subtype,
                               std::size_t extend, bool is_resizeable) noexcept
    : Subtyped(type, subtype)
    , extend_(checked_narrow<std::uint32_t>(extend))
    , is_resizeable_(is_resizeable) {}

  virtual std::size_t size(void const* parent) const noexcept;
  constexpr std::size_t extend() const noexcept {
    return extend_;
  }

  virtual void* data(void* parent) const noexcept = 0;
  virtual void const* data(void const* parent) const noexcept = 0;

  virtual bool resize(void* parent, std::size_t size) const noexcept = 0;

  constexpr bool isResizeable() const noexcept {
    return is_resizeable_;
  }

private:
  std::uint32_t extend_;
  bool is_resizeable_;
};

class IDLE_API(idle) SetType : public Subtyped {
public:
  using IterationCallback = bool (*)(void* context, std::size_t index,
                                     void* value);
  using ConstIterationCallback = bool (*)(void* context, std::size_t index,
                                          void const* value);

  constexpr SetType(MappedType type, Type const* subtype) noexcept
    : Subtyped(type, subtype) {}

  virtual std::size_t size(void const* parent) const noexcept;

  virtual bool contains(void const* parent,
                        void const* value) const noexcept = 0;

  virtual void clear(void* parent) const noexcept = 0;

  virtual bool insert(void* parent, void const* value) const noexcept = 0;
  virtual std::size_t erase(void* parent, void const* value) const noexcept = 0;

  /// \attention Try to avoid the non-const method since it has to create a
  /// copy for each object to check it afterwards for a possible mutation.
  virtual bool iterate(void* parent, std::size_t size, void* context,
                       IterationCallback callback) const noexcept = 0;
  virtual bool iterate(void const* parent, std::size_t size, void* context,
                       ConstIterationCallback callback) const noexcept = 0;
};

class ObjectType : public Type {
  using ReflectionRef = Reflection const&;
  using Fn = ReflectionRef (*)();

public:
  explicit ObjectType(Fn ref) noexcept
    : Type(MappedType::Empty)
    , ref_(ref) {
    IDLE_ASSERT(ref_);
  }

  Reflection const& reflection() const noexcept {
    return ref_();
  }

private:
  Fn ref_;
};
} // namespace art
} // namespace idle

#endif // IDLE_SERVICE_ART_REFLECTION_HPP_INCLUDED
