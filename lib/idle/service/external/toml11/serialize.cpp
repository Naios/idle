
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
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <idle/core/casting.hpp>
#include <idle/service/art/types.hpp>
#include <idle/service/art/visitor.hpp>
#include <idle/service/external/toml11/serialize.hpp>
#include <idle/service/sink.hpp>

using namespace idle::art;

namespace idle {
class TOMLSerializerVisitor {
  using reference = toml::basic_value<toml::preserve_comments>&;

public:
  explicit TOMLSerializerVisitor(reference value, Nullable<Sink>)
    : stack_({value}) {}

  VisitorResult accept(MappedType mapped, void const* primitive,
                       PrimitiveType const* type) {
    if (StringView name = type->name(primitive)) {
      top() = std::string(name.begin(), name.end());
      return VisitorResult::Ok;
    } else {
      return accept(mapped, primitive);
    }
  }
  VisitorResult accept(MappedType mapped, void const* primitive) {
    return type_cast(mapped, primitive, [this](auto* obj) {
      top() = *obj;
      return VisitorResult::Ok;
    });
  }

  VisitorResult peek(MappedType mapped, std::size_t size, ContainerType type) {
    (void)mapped;
    (void)type;

    reference current = top();
    if (!current.is_array()) {
      current = toml::array{};
    }

    auto& array = current.as_array(std::nothrow);

    if (array.size() != size) {
      array.resize(size);
    }

    return VisitorResult::Ok;
  }

  VisitorResult push(MappedType mapped, char const* name,
                     char const* description) {

    (void)mapped;
    (void)description;

    reference current = top();
    if (!current.is_table()) {
      current = toml::table{};
    }

    reference next = current[name];

    if (description) {
      std::vector<std::string> comments;
      boost::algorithm::split(
          comments, std::string(description),
          [](char c) {
            return c == '\n';
          },
          boost::token_compress_on);

      boost::algorithm::trim_if(comments, [](auto const& str) {
        return str.empty();
      });

      for (auto itr = comments.begin(); itr != comments.end();) {
        boost::algorithm::trim(*itr);

        if (itr->empty()) {
          itr = comments.erase(itr);
        } else {
          itr->insert(itr->begin(), ' ');
          ++itr;
        }
      }

      if (!comments.empty()) {
        next.comments() = std::move(comments);
      }
    }

    stack_.emplace_back(next);
    return VisitorResult::Ok;
  }
  VisitorResult push(MappedType mapped, std::size_t index) {
    (void)mapped;

    stack_.emplace_back(top().at(index));
    return VisitorResult::Ok;
  }

  void pop(MappedType type) {
    (void)type;

    IDLE_ASSERT(stack_.size() > 1);
    IDLE_ASSERT(!top().is_uninitialized());

    stack_.pop_back();
  }

  reference top() noexcept {
    IDLE_ASSERT(!stack_.empty());
    return stack_.back().get();
  }

private:
  std::vector<std::reference_wrapper<std::remove_reference_t<reference>>>
      stack_;
};

class TOMLDeserializerVisitor {
  using reference = toml::basic_value<toml::preserve_comments> const&;

public:
  explicit TOMLDeserializerVisitor(reference value, Nullable<Sink> out)
    : stack_({value})
    , out_(out) {}

  VisitorResult accept(MappedType mapped, void* primitive,
                       PrimitiveType const* type) {
    reference current = top();

    if (current.is_string()) {
      if (void const* value = type->value(toml::get<std::string>(top()))) {
        type_copy(mapped, primitive, value);
        return VisitorResult::Ok;
      } else {
        // TODO Error: No repr found
        return VisitorResult::Cancel;
      }
    } else {
      return accept(mapped, primitive);
    }
  }
  VisitorResult accept(MappedType mapped, void* primitive) {
    return type_cast(mapped, primitive, *this);
  }

  VisitorResult peek(MappedType mapped, std::size_t& in_out_size,
                     ContainerType type) {
    auto const& array = top().as_array();
    auto const actual = array.size();

    if ((type == ContainerType::ArrayLike) && (actual != in_out_size)) {
      if (out_) {
        out_->write(toml::format_error(
            "Bad array size", top(),
            fmt::format(FMT_STRING(
                            "Expected an array size of '{}' but got '{}'!"),
                        in_out_size, actual),
            {top().comments().begin(), top().comments().end()}, true));
      }

      return VisitorResult::Cancel;
    }

    for (auto const& element : array) {
      if (!isCompatibleTo(element.type(), mapped)) {
        if (out_) {
          out_->write(toml::format_error(
              "Type is not compatible", top(),
              fmt::format(FMT_STRING("Needs to be '{}'"), mapped), {}, true));
        }

        return VisitorResult::Cancel;
      }
    }

    in_out_size = actual;
    return VisitorResult::Ok;
  }

  VisitorResult push(MappedType mapped, char const* name,
                     char const* description) {

    (void)description;

    reference current = top();

    if (!current.contains(name)) {
      return VisitorResult::Skip;
    }

    IDLE_ASSERT(current.is_table());

    reference next = current.at(name);

    if (!isCompatibleTo(next.type(), mapped)) {
      // TODO Throw an error here
      return VisitorResult::Cancel;
    }

    stack_.push_back(next);
    return VisitorResult::Ok;
  }
  VisitorResult push(MappedType mapped, std::size_t index) {
    (void)mapped;

    stack_.push_back(top().at(index));
    return VisitorResult::Ok;
  }

  void pop(MappedType type) {
    (void)type;

    IDLE_ASSERT(stack_.size() > 1);
    stack_.pop_back();
  }

  reference top() noexcept {
    IDLE_ASSERT(!stack_.empty());
    return stack_.back().get();
  }

  VisitorResult operator()(float* obj) {
    reference value = top();
    if (value.is_floating()) {
      *obj = static_cast<float>(value.as_floating(std::nothrow));
    } else {
      IDLE_ASSERT(value.is_integer());
      *obj = static_cast<float>(value.as_integer(std::nothrow));
    }
    return VisitorResult::Ok;
  }
  VisitorResult operator()(double* obj) {
    reference value = top();
    if (value.is_floating()) {
      *obj = value.as_floating(std::nothrow);
    } else {
      IDLE_ASSERT(value.is_integer());
      *obj = static_cast<double>(value.as_integer(std::nothrow));
    }
    return VisitorResult::Ok;
  }
  template <typename T>
  VisitorResult operator()(T* obj) {
    *obj = toml::get<T>(top());
    return VisitorResult::Ok;
  }

private:
  static bool isCompatibleTo(toml::value_t from, MappedType to) {
    switch (from) {
      case toml::value_t::boolean:
        return isBool(to);
      case toml::value_t::integer:
        return isIntegral(to) || isFloating(to);
      case toml::value_t::floating:
        return isFloating(to);
      case toml::value_t::string:
        return isString(to);
      case toml::value_t::array:
        return isArray(to);
      case toml::value_t::table:
        return isObject(to);
      default:
        return false;
    }
  }

  std::vector<std::reference_wrapper<std::remove_reference_t<reference>>>
      stack_;
  Nullable<Sink> out_;
};

void toml_serialize(toml::basic_value<toml::preserve_comments>& toml,
                    ConstReflectionPtr ptr) {
  TOMLSerializerVisitor visitor(toml, {});
  art::reflection_visit(visitor, ptr);
}

bool toml_deserialize(toml::basic_value<toml::preserve_comments> const& toml,
                      ReflectionPtr ptr, Nullable<Sink> out) {
  TOMLDeserializerVisitor visitor(toml, out);
  return art::reflection_visit(visitor, ptr);
}
} // namespace idle
