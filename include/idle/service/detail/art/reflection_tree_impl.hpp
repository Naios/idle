
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

#ifndef IDLE_SERVICE_DETAIL_ART_REFLECTION_TREE_IMPL_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_ART_REFLECTION_TREE_IMPL_HPP_INCLUDED

#include <array>
#include <iterator>
#include <memory>
#include <utility>
#include <idle/core/util/enum.hpp>
#include <idle/core/util/enum_map.hpp>
#include <idle/core/util/enumerate.hpp>
#include <idle/core/util/meta.hpp>
#include <idle/service/art/reflection.hpp>
#include <idle/service/art/types.hpp>

namespace idle {
namespace art {
namespace detail {
template <typename T, typename = void>
struct SuperGetter {
  static constexpr Nullable<Reflection const> get() noexcept {
    return {};
  }
};
template <typename T>
struct SuperGetter<
    T, void_t<decltype(reflect(std::declval<typename T::Super const*>()))>> {
  static constexpr Nullable<Reflection const> get() noexcept {
    static_assert(static_cast<typename T::Super*>(static_cast<T*>(nullptr)) ==
                      static_cast<typename T::Super*>(nullptr),
                  "T::Super is not the primary base class of T and can't be "
                  "reflected as such. Make sure that T::Super is the first "
                  "inherited class of T!");

    return reflect(static_cast<typename T::Super const*>(nullptr));
  }
};

template <typename T, std::size_t Size>
class ReflectionImpl final : public Reflection {
  using Fields = std::array<FieldType, Size>;

public:
  explicit constexpr ReflectionImpl(Fields fields)
    : Reflection()
    , fields_(fields) {}

  Span<FieldType const> fields() const noexcept override {
    return fields_;
  }

  Nullable<Reflection const> super() const noexcept override {
    return SuperGetter<T>::get();
  }

private:
  Fields fields_;
};
template <typename T>
class ReflectionImpl<T, 0> final : public Reflection {
public:
  explicit constexpr ReflectionImpl(std::array<FieldType, 0>)
    : Reflection() {}

  Span<FieldType const> fields() const noexcept override {
    return {};
  }

  Nullable<Reflection const> super() const noexcept override {
    return SuperGetter<T>::get();
  }
};

template <typename>
class PrimitiveTypeImpl;
template <typename>
class ArrayTypeImpl;
template <typename>
class SetTypeImpl;
template <typename>
class ObjectTypeImpl;

template <typename T>
constexpr Type const* primitiveSelect(std::false_type, identity<T>) noexcept {
  return nullptr;
}
template <typename T>
constexpr Type const* primitiveSelect(std::true_type, identity<T>) noexcept {
  return &(PrimitiveTypeImpl<T>::instance());
}

template <MappedType Mapped, typename T>
constexpr Type const* subtypeSelect(std::integral_constant<MappedType, Mapped>,
                                    identity<T> id) noexcept {
  return primitiveSelect(std::is_enum<T>{}, id);
}
template <typename T>
constexpr Type const*
subtypeSelect(std::integral_constant<MappedType, MappedType::Array>,
              identity<T>) noexcept {
  return &(ArrayTypeImpl<T>::instance());
}
template <typename T>
constexpr Type const*
subtypeSelect(std::integral_constant<MappedType, MappedType::Set>,
              identity<T>) noexcept {
  return &(SetTypeImpl<T>::instance());
}
template <typename T>
constexpr Type const*
subtypeSelect(std::integral_constant<MappedType, MappedType::Object>,
              identity<T>) noexcept {
  return &(ObjectTypeImpl<T>::instance());
}

template <typename T, typename = void>
struct Resizer : std::false_type {
  static constexpr bool resize(T&, std::size_t) noexcept {
    return false;
  }
};
template <typename T>
struct Resizer<T, void_t<decltype(std::declval<T>().resize(0))>>
  : std::true_type {
  static constexpr bool resize(T& container, std::size_t size) noexcept {
    container.resize(size);
    return true;
  }
};

template <typename T>
class ArrayTypeImpl final : public ArrayType {
  using value_type = unrefcv_t<decltype(*::idle::art::detail::data(
      std::declval<T&>()))>;
  using resizer_type = Resizer<T>;

public:
  constexpr ArrayTypeImpl() noexcept
    : ArrayType(TypeOf<value_type>::value,
                subtypeSelect(TypeOf<value_type>{}, identity<value_type>{}),
                sizeof(value_type), resizer_type::value) {}

  void* data(void* parent) const noexcept override {
    return ::idle::art::detail::data(*static_cast<T*>(parent));
  }
  void const* data(void const* parent) const noexcept override {
    return ::idle::art::detail::data(*static_cast<T const*>(parent));
  }
  std::size_t size(void const* parent) const noexcept override {
    return std::cend(*static_cast<T const*>(parent)) -
           std::cbegin(*static_cast<T const*>(parent));
  }

  bool resize(void* parent, std::size_t size) const noexcept override {
    return resizer_type::resize(*static_cast<T*>(parent), size);
  }

  static Type const& instance() noexcept {
    static ArrayTypeImpl const inst;
    return inst;
  }
};

template <typename T>
class SetTypeImpl final : public SetType {
  using value_type = unrefcv_t<decltype(*std::begin(std::declval<T&>()))>;

public:
  constexpr SetTypeImpl() noexcept
    : SetType(TypeOf<value_type>::value,
              subtypeSelect(TypeOf<value_type>{}, identity<value_type>{})) {}

  std::size_t size(void const* parent) const noexcept override {
    return static_cast<T const*>(parent)->size();
  }

  bool contains(void const* parent, void const* value) const noexcept override {
    T const& me = *static_cast<T const*>(parent);
    return me.find(*static_cast<value_type const*>(value)) != me.end();
  }

  void clear(void* parent) const noexcept override {
    static_cast<T*>(parent)->clear();
  }

  bool insert(void* parent, void const* value) const noexcept override {
    return static_cast<T*>(parent)
        ->insert(*static_cast<value_type const*>(value))
        .second;
  }

  std::size_t erase(void* parent, void const* value) const noexcept override {
    return static_cast<T*>(parent)->erase(
        *static_cast<value_type const*>(value));
  }

  bool iterate(void* parent, std::size_t size, void* context,
               IterationCallback callback) const noexcept override {
    T& me = *static_cast<T*>(parent);

    std::size_t i = 0;

    auto current = me.begin();
    auto const end = me.end();

    // Perform a safe iteration (allows erasure of iterated values)
    while ((current != end) && (i < size)) {
      value_type cached = *current;
      auto const previous = current;

      ++current;

      if (!callback(context, i, std::addressof(cached))) {
        return false;
      }

      if (cached != *previous) {
        // Reinsert the element if it was mutated
        me.erase(previous);

        current = me.insert(cached).first;
        ++current;
      }

      ++i;
    }

    for (; i < size; ++i) {
      value_type cached;
      if (!callback(context, i, std::addressof(cached))) {
        return false;
      }
      me.insert(cached);
    }
    return true;
  }

  bool iterate(void const* parent, std::size_t size, void* context,
               ConstIterationCallback callback) const noexcept override {
    T const& me = *static_cast<T const*>(parent);

    IDLE_ASSERT(me.size() == size);

    std::size_t i = 0;

    auto current = me.begin();
    auto const end = me.end();

    // Perform a safe iteration (allows erasure of iterated values)
    while ((current != end) && (i < size)) {
      std::conditional_t<std::is_lvalue_reference<decltype(*current)>::value,
                         value_type const&, value_type>
          value = *current;

      ++current;

      if (!callback(context, i, std::addressof(value))) {
        return false;
      }

      ++i;
    }
    return true;
  }

  static Type const& instance() noexcept {
    static SetTypeImpl const inst;
    return inst;
  }
};

template <typename Enum>
constexpr auto enum_entries(Enum const& enumeration) noexcept {
  Array<PrimitiveType::Entry, Enum::size> buffer;
  for (std::size_t i = 0; i < Enum::size; ++i) {
    auto const* const entry = enumeration[i];
    buffer[i] = PrimitiveType::Entry{enumeration.name(*entry), entry->ptr()};
  }
  return buffer;
}

template <typename T>
class PrimitiveTypeImpl final : public PrimitiveType {
  struct Enumeration {
    decltype(enumerate<T>()) enumeration{enumerate<T>()};
    decltype(enum_entries(enumeration)) entries{enum_entries(enumeration)};
    decltype(enum_values(enumeration)) values{enum_values(enumeration)};
    decltype(enum_names(enumeration)) names{enum_names(enumeration)};
  };

public:
  constexpr PrimitiveTypeImpl() noexcept
    : PrimitiveType(TypeOf<T>::value, my_enum().enumeration.size,
                    my_enum().entries.data(), my_enum().values.data(),
                    my_enum().names.data()) {}

  static Type const& instance() noexcept {
    static PrimitiveTypeImpl const inst;
    return inst;
  }

private:
  static Enumeration const& my_enum() noexcept {
    static Enumeration inst;
    return inst;
  }
};

template <typename T>
class ObjectTypeImpl final : public ObjectType {
public:
  constexpr ObjectTypeImpl() noexcept
    : ObjectType(&getter) {}

  static Type const& instance() noexcept {
    static ObjectTypeImpl const inst;
    return inst;
  }

private:
  static constexpr Reflection const& getter() noexcept {
    return reflect(static_cast<T const*>(nullptr));
  }
};
} // namespace detail
} // namespace art
} // namespace idle

#ifdef offsetof
#  define IDLE_DETAIL_OFFSETOF(CLASS, MEMBER) offsetof(CLASS, MEMBER)
#else
#  define IDLE_DETAIL_OFFSETOF(CLASS, MEMBER)                                  \
    reinterpret_cast<std::size_t>(                                             \
        std::addressof((static_cast<CLASS*>(nullptr)->MEMBER)))
#endif

#endif // IDLE_SERVICE_DETAIL_ART_REFLECTION_TREE_IMPL_HPP_INCLUDED
