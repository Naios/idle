
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

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <idle/core/detail/algorithm.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/util/algorithm.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/enum_map.hpp>
#include <idle/core/util/meta.hpp>
#include <idle/core/util/printable.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/service/art/reflection.hpp>
#include <idle/service/art/types.hpp>
#include <idle/service/art/visitor.hpp>

namespace idle {
std::ostream& art::operator<<(std::ostream& os, MappedType value) {
  switch (value) {
#define IDLE_DETAIL_FOR_EACH_MAPPED_TYPE(NAME)                                 \
  case MappedType::NAME:                                                       \
    os << #NAME;                                                               \
    return os;
#include <idle/service/art/reflection.inl>
  }

  IDLE_DETAIL_UNREACHABLE();
}

Nullable<art::FieldType const>
Reflection::find(StringView name) const noexcept {
  /*for (FieldType const& field : fields()) {
    return {}; // TODO
  }*/

  (void)name;
  IDLE_DETAIL_UNREACHABLE();
  return {};
}

class PrettyPrintVisitor {
  auto indentation() const {
    return printable([=](std::ostream& os) {
      for (std::size_t i = 0; i < indentation_; ++i) {
        os.write("  ", 2);
      }
    });
  }

  static auto print_value(art::MappedType type, void const* value) {
    return printable([=](std::ostream& os) {
      art::type_cast(type, value, [&](auto const* obj) {
        fmt::print(os, FMT_STRING("{}"), *obj);
      });
    });
  }

public:
  explicit PrettyPrintVisitor(std::ostream& os)
    : os_(&os) {}

  art::VisitorResult accept(art::MappedType mapped, void const* primitive,
                            art::PrimitiveType const* type) {
    if (StringView name = type->name(primitive)) {
      fmt::print(out(), FMT_STRING("{}: Enum({})\n"), name,
                 print_value(mapped, primitive));
      return art::VisitorResult::Ok;
    } else {
      return accept(mapped, primitive);
    }
  }
  art::VisitorResult accept(art::MappedType mapped, void const* primitive) {
    fmt::print(out(), FMT_STRING("{}: {}\n"), print_value(mapped, primitive),
               mapped);
    return art::VisitorResult::Ok;
  }

  art::VisitorResult peek(art::MappedType mapped, std::size_t size,
                          art::ContainerType type) {
    (void)mapped;
    (void)size;
    (void)type;

    static constexpr auto enumeration = enumerate<art::ContainerType>();

    fmt::print(out(), FMT_STRING("(Size: {}, Type: {}) = {}\n"), size,
               enum_name(enumeration, type), "{");

    return art::VisitorResult::Ok;
  }

  art::VisitorResult push(art::MappedType type, char const* name,
                          char const* description) {
    (void)description;

    /*if (description) {
      fmt::print(out(), FMT_STRING("#{} {}\n"), indentation(), description);
    }*/

    if (art::isStructured(type)) {
      if (art::isArray(type) || art::isSet(type)) {
        fmt::print(out(), FMT_STRING("{}{}: {}"), indentation(), name, type);
      } else {
        fmt::print(out(), FMT_STRING("{}{}: {} = {}\n"), indentation(), name,
                   type, "{");
      }
    } else {
      IDLE_ASSERT(art::isPrimitive(type));

      fmt::print(out(), FMT_STRING("{}{}: {} = "), indentation(), name, type);
    }

    ++indentation_;

    return art::VisitorResult::Ok;
  }
  art::VisitorResult push(art::MappedType type, std::size_t index) {
    (void)index;

    if (art::isStructured(type)) {
      fmt::print(out(), FMT_STRING("{}{}: {}\n"), indentation(), index, "{");
    } else {
      fmt::print(out(), FMT_STRING("{}{}: "), indentation(), index);
    }

    ++indentation_;
    return art::VisitorResult::Ok;
  }

  void pop(art::MappedType type) {
    IDLE_ASSERT(indentation_ > 0);

    --indentation_;

    if (art::isStructured(type)) {
      fmt::print(out(), FMT_STRING("{}{}\n"), indentation(), "}");
    }
  }

private:
  std::ostream& out() const noexcept {
    return *os_;
  }

  std::ostream* os_;
  std::size_t indentation_{0};
};

std::ostream& operator<<(std::ostream& os, ReflectionPtr const& ptr) {
  return os << ConstReflectionPtr(ptr);
}

std::ostream& operator<<(std::ostream& os, ConstReflectionPtr const& ptr) {
  PrettyPrintVisitor vis(os);
  art::reflection_visit(vis, ptr);
  return os;
}

namespace art {
/// Is necessary for deduplicating uint32/uint64 with its index type std::size_t
template <typename T>
class Wrap {
public:
  explicit Wrap(T const& value_) noexcept
    : value(&value_) {}

  T const* value;
};

template <typename T>
struct ValueReflectionLess {
  constexpr bool operator()(std::size_t left,
                            Wrap<T> const& wrap) const noexcept {
    return *static_cast<T const*>((*primitive)[left].value) < *wrap.value;
  }
  constexpr bool operator()(Wrap<T> const& wrap,
                            std::size_t right) const noexcept {
    return *wrap.value < *static_cast<T const*>((*primitive)[right].value);
  }
  constexpr bool operator()(std::size_t left,
                            std::size_t right) const noexcept {
    return *static_cast<T const*>((*primitive)[left].value) <
           *static_cast<T const*>((*primitive)[right].value);
  }

  PrimitiveType const* primitive;
};

struct NameReflectionLess {
  constexpr bool operator()(std::size_t left,
                            std::size_t right) const noexcept {
    return (*primitive)[left].name < (*primitive)[right].name;
  }
  constexpr bool operator()(std::size_t left, StringView str) const noexcept {
    return (*primitive)[left].name < str;
  }
  constexpr bool operator()(StringView str, std::size_t right) const noexcept {
    return str < (*primitive)[right].name;
  }

  PrimitiveType const* primitive;
};

void const* PrimitiveType::value(StringView name) const noexcept {
  auto const my_names = names();

  auto const itr = find_first_lower_bound_of(my_names.begin(), my_names.end(),
                                             name, NameReflectionLess{this});

  if (itr != my_names.end()) {
    return (*this)[*itr].value;
  } else {
    return nullptr;
  }
}

StringView PrimitiveType::name(void const* value) const noexcept {
  auto const my_values = values();

  return type_cast(type(), value, [&](auto const* obj) -> StringView {
    using type_t = std::remove_pointer_t<decltype(obj)>;

    auto const itr = find_first_lower_bound_of(
        my_values.begin(), my_values.end(), Wrap<type_t>(*obj),
        ValueReflectionLess<type_t>{this});

    if (itr != my_values.end()) {
      return (*this)[*itr].name;
    } else {
      return {};
    }
  });
}

std::size_t ArrayType::size(void const* parent) const noexcept {
  (void)parent;

  IDLE_DETAIL_UNREACHABLE(); // Anchors the VTable into the CU
}

std::size_t SetType::size(void const* parent) const noexcept {
  (void)parent;

  IDLE_DETAIL_UNREACHABLE(); // Anchors the VTable into the CU
}
} // namespace art
} // namespace idle
