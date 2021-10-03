
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

#include <iterator>
#include <catch2/catch.hpp>
#include <idle/core/util/deref.hpp>

using namespace idle;

static constexpr int magic = 37454;

struct A {
  int testA() {
    return magic;
  }
};

struct B {
  A a{};

  A* operator->() {
    return &a;
  }
};

struct C {
  B b{};

  B& operator->() {
    return b;
  }
};

TEST_CASE("Can deref", "[deref]") {
  A a;
  B b;
  C c;

  REQUIRE(deref(a).testA() == magic);
  REQUIRE(deref(&a).testA() == magic);

  REQUIRE(deref(b).testA() == magic);

  REQUIRE(deref(c).testA() == magic);
}
