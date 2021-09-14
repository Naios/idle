
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

#ifndef IDLE_SERVICE_ART_VISITOR_HPP_INCLUDED
#define IDLE_SERVICE_ART_VISITOR_HPP_INCLUDED

#include <idle/core/util/meta.hpp>
#include <idle/service/art/reflection.hpp>
#include <idle/service/art/types.hpp>

#define VIS_TRY_RETURN(EXPR)                                                   \
  do {                                                                         \
    switch ((EXPR)) {                                                          \
      case ::idle::art::VisitorResult::Ok:                                     \
        break;                                                                 \
      case ::idle::art::VisitorResult::Skip:                                   \
        return VisitorResult::Ok;                                              \
      case ::idle::art::VisitorResult::Cancel:                                 \
        return VisitorResult::Cancel;                                          \
    }                                                                          \
  } while (false)
#define VIS_TRY_CONTINUE(EXPR)                                                 \
  switch ((EXPR)) {                                                            \
    case ::idle::art::VisitorResult::Ok:                                       \
      break;                                                                   \
    case ::idle::art::VisitorResult::Skip:                                     \
      return VisitorResult::Ok;                                                \
    case ::idle::art::VisitorResult::Cancel:                                   \
      return VisitorResult::Cancel;                                            \
  }

namespace idle {
namespace art {
enum class VisitorResult : std::uint8_t { Ok, Skip, Cancel };

enum class ContainerType : std::uint8_t {
  /// A fixed size (non resizeable) continuous array
  ArrayLike,

  /// A resizeable continuous array
  VectorLike,

  /// An ordered data structure capable of holding an arbitrary amount of values
  SetLike
};

namespace detail {
inline bool resize_impl(ArrayType const& type, void* obj, std::size_t size) {
  return type.resize(obj, size);
}
inline bool resize_impl(ArrayType const& type, void const* obj,
                        std::size_t size) {
  (void)type;
  (void)obj;
  (void)size;
  return false;
}

inline void clear_impl(SetType const& type, void* obj) {
  type.clear(obj);
}
inline void clear_impl(SetType const& type, void const* obj) {
  (void)type;
  (void)obj;
}

template <typename Visitor, typename VType>
class VisitorImpl {
  using pointer = VType;

  struct IterationContext {
    VisitorImpl* me;
    SetType const* type;
  };

public:
  explicit VisitorImpl(Visitor&& visitor)
    : visitor_(std::forward<Visitor>(visitor)) {}

  template <typename SType>
  VisitorResult visit(pointer obj, SType const& type) {
    MappedType const mapped = type.type();

    if (isPrimitive(mapped)) {
      if (type.hasSubtype()) {
        return visitor_.accept(mapped, obj,
                               &static_cast<PrimitiveType const&>(
                                   type.subtype()));
      } else {
        return visitor_.accept(mapped, obj);
      }
    } else if (isArray(mapped)) {
      return visitArray(obj, static_cast<ArrayType const&>(type.subtype()));
    } else if (isSet(mapped)) {
      return visitSet(obj, static_cast<SetType const&>(type.subtype()));
    } else {
      IDLE_ASSERT(isObject(mapped));
      return visitObject(
          obj, static_cast<ObjectType const&>(type.subtype()).reflection());
    }
  }
  VisitorResult visitArray(pointer obj, ArrayType const& type) {
    auto const actual = type.size(obj);
    auto size = actual;

    MappedType const mapped = type.type();

    VIS_TRY_RETURN(visitor_.peek(mapped, size,
                                 type.isResizeable()
                                     ? ContainerType::VectorLike
                                     : ContainerType::ArrayLike));

    if (size != actual) {
      if (!resize_impl(type, obj, size)) {
        return VisitorResult::Cancel;
      }
    }

    IDLE_ASSERT(type.size(obj) == size);

    if (size) {
      auto* const data = type.data(obj);
      auto const extend = type.extend();

      std::size_t i = 0;
      pointer itr = data;

      for (; i != size; ++i, itr = reinterpret_cast<pointer>(
                                 reinterpret_cast<std::size_t>(itr) + extend)) {
        VIS_TRY_RETURN(visitor_.push(mapped, i));
        VisitorResult const result = visit(itr, type);
        visitor_.pop(type.type());

        VIS_TRY_RETURN(result);
      }
    }

    return VisitorResult::Ok;
  }

  VisitorResult visitSet(pointer obj, SetType const& type) {
    auto const actual = type.size(obj);
    auto size = actual;

    VIS_TRY_RETURN(visitor_.peek(type.type(), size, ContainerType::SetLike));

    if (size < actual) {
      clear_impl(type, obj);
    }

    if (size) {
      auto const callback = [](void* context, std::size_t i,
                               auto* value) -> bool {
        return ([&] {
          IterationContext& c = *static_cast<IterationContext*>(context);

          VIS_TRY_RETURN(c.me->visitor_.push(c.type->type(), i));
          VisitorResult const result = c.me->visit(value, *c.type);
          c.me->visitor_.pop(c.type->type());

          VIS_TRY_RETURN(result);

          return VisitorResult::Ok;
        }() == VisitorResult::Ok);
      };

      IterationContext my_context{this, &type};

      if (type.iterate(obj, size, &my_context, callback)) {
        return VisitorResult::Ok;
      } else {
        return VisitorResult::Cancel;
      }
    }

    return VisitorResult::Ok;
  }
  VisitorResult visitObject(pointer obj, Reflection const& reflection) {
    if (auto const super = reflection.super()) {
      VIS_TRY_RETURN(visitObject(obj, *super));
    }

    for (FieldType const& field : reflection.fields()) {
      MappedType const type = field.type();

      VIS_TRY_CONTINUE(visitor_.push(type, //
                                     field.name(), field.description()));

      VisitorResult const result = visit(field.relocate(obj), field);
      visitor_.pop(type);

      VIS_TRY_CONTINUE(result);
    }

    return VisitorResult::Ok;
  }

private:
  Visitor&& visitor_;
};
} // namespace detail

/// Traverses the Reflection tree with the given Visitor
///
/// ```cpp
/// struct Visitor {
///   VisitorResult accept(MappedType mapped, void* primitive);
///   VisitorResult accept(MappedType mapped, void* primitive,
///                        PrimitiveType const* type);
///
///   VisitorResult peek(MappedType mapped, std::size_t& in_out_capacity,
///                      ContainerType type);
///
///   VisitorResult push(MappedType mapped, char const* name,
///                      char const* description);
///   VisitorResult push(std::size_t index);
///
///   void pop(MappedType mapped);
/// };
/// ```
template <typename Visitor>
bool reflection_visit(Visitor&& visitor, ReflectionPtr ptr) {
  detail::VisitorImpl<Visitor&&, ReflectionPtr::pointer> helper(
      std::forward<Visitor>(visitor));
  return helper.visitObject(ptr.object(), ptr.reflection()) !=
         VisitorResult::Cancel;
}
/// \copydoc reflection_visit
template <typename Visitor>
bool reflection_visit(Visitor&& visitor, ConstReflectionPtr ptr) {
  detail::VisitorImpl<Visitor&&, ConstReflectionPtr::pointer> helper(
      std::forward<Visitor>(visitor));
  return helper.visitObject(ptr.object(), ptr.reflection()) !=
         VisitorResult::Cancel;
}
} // namespace art
} // namespace idle

#undef VIS_TRY_RETURN
#undef VIS_TRY_CONTINUE

#endif // IDLE_SERVICE_ART_VISITOR_HPP_INCLUDED
