
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
#include <idle/core/util/enum_map.hpp>
#include <idle/core/util/flat_set.hpp>
#include <idle/core/util/functional.hpp>

using namespace idle;

TEST_CASE("flat-set behaves correctly", "[flat-set]") {
  FlatSet<int, std::greater<>> set;

  auto contains = [&](int i) {
    return set.find(i) != set.end();
  };

  REQUIRE(!contains(4));
  set.insert(4);
  REQUIRE(contains(4));
  set.insert(2);
  REQUIRE(contains(2));
  set.insert(2);
  set.insert(2);
  set.insert(2);
  REQUIRE(!contains(28));
  set.insert(28);
  REQUIRE(contains(28));
  REQUIRE(!contains(100));
  set.insert(100);
  REQUIRE(contains(100));
  REQUIRE(!contains(1));
  set.insert(1);
  REQUIRE(contains(1));

  set.erase(99);

  set.erase(28);

  set.erase(1);

  set.erase(100);

  set.erase(2);

  set.erase(4);

  REQUIRE(set.empty());

  set.insert(1);
  set.insert(3);
  set.insert(2);
  set.insert(5);
  set.insert(4);
  REQUIRE(set.size() == 5);

  {
    auto last = std::prev(set.end());
    set.erase(last);
  }

  {
    auto lastast = std::prev(std::prev(set.end()));
    set.erase(lastast);
    REQUIRE(set.size() == 3);
  }
}
