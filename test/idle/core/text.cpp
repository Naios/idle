
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
#include <idle/core/util/string_view.hpp>
#include <idle/core/util/text.hpp>

using namespace idle;

TEST_CASE("Can parse raw string literals", "[text]") {
  REQUIRE(R"(

  )"_txt == ""_sv);

  REQUIRE(R"(
test
  )"_txt == "test"_sv);
}

TEST_CASE("Can trim", "[text]") {
  REQUIRE(trim(" \r\n \r\ntest") == "test"_sv);
  REQUIRE(trim("test") == "test"_sv);
  REQUIRE(trim("test \r\n \r\n    ") == "test"_sv);
  REQUIRE(trim(" \r\n   \r\n  test \r\n \r\n    ") == "test"_sv);
}
