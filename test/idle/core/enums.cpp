
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

#include <catch2/catch.hpp>
#include <idle/core/util/enum.hpp>
#include <idle/core/util/enum_map.hpp>
#include <idle/core/util/enumerate.hpp>

using namespace idle;

enum class MyEmptyEnum : unsigned {};

namespace testxyz {
enum class MyDenseEnum : unsigned {
  One,
  Two,
  Three,
  A4,
  A5,
  A6,
  A7,
  A8,
  A9,
  A10,
  A11,
  A12,
};

namespace zxy {
enum class MyMaskEnum : unsigned {
  A = 1 << 0,
  B = 1 << 1,
  C = 1 << 2,
  D = 1 << 3,
  E = 1 << 4,
  F = 1 << 5,
  G = 1 << 6,
  H = 1 << 7,
};
}
} // namespace testxyz

enum class MySparseEnum : unsigned {
  One = 10,
  Two = 20,
  Three = 30,
};

using namespace testxyz;
using namespace zxy;

namespace idle {
template <>
struct enum_trait<MyMaskEnum> : enum_mask<MyMaskEnum> {};
} // namespace idle

TEST_CASE("Have correct size", "[enums]") {
  REQUIRE(enum_size<MyEmptyEnum>() == 0);
  REQUIRE(enum_size<MyDenseEnum>() == 12);
  REQUIRE(enum_size<MySparseEnum>() == 3);
  REQUIRE(enum_size<MyMaskEnum>() == 8);
}

TEST_CASE("Detects correct density", "[enums]") {
  REQUIRE(is_enum_dense<MyDenseEnum>::value);
  REQUIRE(!is_enum_dense<MySparseEnum>::value);
  REQUIRE(is_enum_dense<MyEmptyEnum>::value);
}

TEST_CASE("Can name the values", "[enums]") {
  SECTION("dense") {
    static constexpr auto enumeration = enumerate<MyDenseEnum>();
    REQUIRE(enum_name(enumeration, MyDenseEnum::Three) == "Three");
    REQUIRE(enum_name(enumeration, MyDenseEnum::A11) == "A11");
  }

  SECTION("sparse") {
    static constexpr auto enumeration = enumerate<MySparseEnum>();
    static constexpr StringView name = enum_name(enumeration,
                                                 MySparseEnum::Two);
    REQUIRE(name == "Two");
  }
}

TEST_CASE("Can uppercase name the values", "[enums]") {
  SECTION("dense") {
    static constexpr auto
        enumeration = enumerate<MyDenseEnum, enum_uppercase<MyDenseEnum>>();
    REQUIRE(enum_name(enumeration, MyDenseEnum::Three) == "THREE");
    REQUIRE(enum_name(enumeration, MyDenseEnum::A11) == "A11");
  }

  SECTION("sparse") {
    static constexpr auto
        enumeration = enumerate<MySparseEnum, enum_uppercase<MyDenseEnum>>();
    static constexpr StringView name = enum_name(enumeration,
                                                 MySparseEnum::Two);
    REQUIRE(name == "TWO");
  }
}

TEST_CASE("Present enum names can be found", "[enums]") {
  constexpr auto enumeration = enumerate<MySparseEnum>();
  constexpr auto index = enum_find(enumeration, "Three");
  constexpr auto value = enumeration.value(*index);

  REQUIRE(index);
  REQUIRE(enumeration.value(*index) == MySparseEnum::Three);
  REQUIRE(value == MySparseEnum::Three);
}

TEST_CASE("Non existing enum names can not be found", "[enums]") {
  SECTION("dense") {
    static constexpr auto enumeration = enumerate<MyDenseEnum>();

    REQUIRE(!enum_find(enumeration, "None"));
    REQUIRE(!enum_find(enumeration, ""));
  }

  SECTION("sparse") {
    static constexpr auto enumeration = enumerate<MySparseEnum>();

    REQUIRE(!enum_find(enumeration, "None"));
    REQUIRE(!enum_find(enumeration, ""));
  }

  SECTION("empty") {
    static constexpr auto enumeration = enumerate<MyEmptyEnum>();

    REQUIRE(!enum_find(enumeration, "None"));
    REQUIRE(!enum_find(enumeration, ""));
  }
}
