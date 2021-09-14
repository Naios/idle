
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

#ifndef IDLE_CORE_DETAIL_MURMUR3_HPP_INCLUDED
#define IDLE_CORE_DETAIL_MURMUR3_HPP_INCLUDED

#include <cstddef>
#include <cstdint>

namespace idle {
namespace detail {
// Implements a constexpr capable murmurhash3 based on:
// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
//
// murmurhash3 is licensed with the following license:
// ```
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
// ```

/*
constexpr std::uint32_t rotl32(std::uint32_t x, std::int8_t r) noexcept {
  return (x << r) | (x >> (32 - r));
}
constexpr std::uint64_t rotl64(std::uint64_t x, std::int8_t r) noexcept {
  return (x << r) | (x >> (64 - r));
}

constexpr std::uint32_t getblock32(std::uint32_t const* p,
                                   std::size_t i) noexcept {
  return p[i];
}

constexpr std::uint64_t getblock64(std::uint64_t const* p,
                                   std::size_t i) noexcept {
  return p[i];
}

constexpr std::uint32_t fmix32(std::uint32_t h) noexcept {
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

constexpr std::uint64_t fmix64(std::uint64_t k) noexcept {
  k ^= k >> 33;
  k *= static_cast<std::uint64_t>(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= static_cast<std::uint64_t>(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

constexpr void MurmurHash3_x86_32(void const* key, std::size_t length,
                                  uint32_t seed, void* out) noexcept {
  // init
  const uint8_t* data = (const uint8_t*)key;
  const int nblocks = length / 4;

  uint32_t h1 = seed;

  const uint32_t c1 = 0xcc9e2d51;
  const uint32_t c2 = 0x1b873593;

  //----------
  // body

  const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);

  for (int i = -nblocks; i; i++) {
    uint32_t k1 = getblock32(blocks, i);

    k1 *= c1;
    k1 = rotl32(k1, 15);
    k1 *= c2;

    h1 ^= k1;
    h1 = rotl32(h1, 13);
    h1 = h1 * 5 + 0xe6546b64;
  }

  //----------
  // tail

  const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);

  uint32_t k1 = 0;

  switch (length & 3) {
    case 3:
      k1 ^= tail[2] << 16;
    case 2:
      k1 ^= tail[1] << 8;
    case 1:
      k1 ^= tail[0];
      k1 *= c1;
      k1 = rotl32(k1, 15);
      k1 *= c2;
      h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= length;

  h1 = fmix32(h1);

  *(uint32_t*)out = h1;
}

void MurmurHash3_x86_128(const void* key, const int len, uint32_t seed,
                         void* out) noexcept {
  const uint8_t* data = (const uint8_t*)key;
  const int nblocks = len / 16;

  uint32_t h1 = seed;
  uint32_t h2 = seed;
  uint32_t h3 = seed;
  uint32_t h4 = seed;

  const uint32_t c1 = 0x239b961b;
  const uint32_t c2 = 0xab0e9789;
  const uint32_t c3 = 0x38b34ae5;
  const uint32_t c4 = 0xa1e38b93;

  //----------
  // body

  const uint32_t* blocks = (const uint32_t*)(data + nblocks * 16);

  for (int i = -nblocks; i; i++) {
    uint32_t k1 = getblock32(blocks, i * 4 + 0);
    uint32_t k2 = getblock32(blocks, i * 4 + 1);
    uint32_t k3 = getblock32(blocks, i * 4 + 2);
    uint32_t k4 = getblock32(blocks, i * 4 + 3);

    k1 *= c1;
    k1 = rotl32(k1, 15);
    k1 *= c2;
    h1 ^= k1;

    h1 = rotl32(h1, 19);
    h1 += h2;
    h1 = h1 * 5 + 0x561ccd1b;

    k2 *= c2;
    k2 = rotl32(k2, 16);
    k2 *= c3;
    h2 ^= k2;

    h2 = rotl32(h2, 17);
    h2 += h3;
    h2 = h2 * 5 + 0x0bcaa747;

    k3 *= c3;
    k3 = rotl32(k3, 17);
    k3 *= c4;
    h3 ^= k3;

    h3 = rotl32(h3, 15);
    h3 += h4;
    h3 = h3 * 5 + 0x96cd1c35;

    k4 *= c4;
    k4 = rotl32(k4, 18);
    k4 *= c1;
    h4 ^= k4;

    h4 = rotl32(h4, 13);
    h4 += h1;
    h4 = h4 * 5 + 0x32ac3b17;
  }

  //----------
  // tail

  const uint8_t* tail = (const uint8_t*)(data + nblocks * 16);

  uint32_t k1 = 0;
  uint32_t k2 = 0;
  uint32_t k3 = 0;
  uint32_t k4 = 0;

  switch (len & 15) {
    case 15:
      k4 ^= tail[14] << 16;
    case 14:
      k4 ^= tail[13] << 8;
    case 13:
      k4 ^= tail[12] << 0;
      k4 *= c4;
      k4 = rotl32(k4, 18);
      k4 *= c1;
      h4 ^= k4;

    case 12:
      k3 ^= tail[11] << 24;
    case 11:
      k3 ^= tail[10] << 16;
    case 10:
      k3 ^= tail[9] << 8;
    case 9:
      k3 ^= tail[8] << 0;
      k3 *= c3;
      k3 = rotl32(k3, 17);
      k3 *= c4;
      h3 ^= k3;

    case 8:
      k2 ^= tail[7] << 24;
    case 7:
      k2 ^= tail[6] << 16;
    case 6:
      k2 ^= tail[5] << 8;
    case 5:
      k2 ^= tail[4] << 0;
      k2 *= c2;
      k2 = rotl32(k2, 16);
      k2 *= c3;
      h2 ^= k2;

    case 4:
      k1 ^= tail[3] << 24;
    case 3:
      k1 ^= tail[2] << 16;
    case 2:
      k1 ^= tail[1] << 8;
    case 1:
      k1 ^= tail[0] << 0;
      k1 *= c1;
      k1 = rotl32(k1, 15);
      k1 *= c2;
      h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len;
  h2 ^= len;
  h3 ^= len;
  h4 ^= len;

  h1 += h2;
  h1 += h3;
  h1 += h4;
  h2 += h1;
  h3 += h1;
  h4 += h1;

  h1 = fmix32(h1);
  h2 = fmix32(h2);
  h3 = fmix32(h3);
  h4 = fmix32(h4);

  h1 += h2;
  h1 += h3;
  h1 += h4;
  h2 += h1;
  h3 += h1;
  h4 += h1;

  ((uint32_t*)out)[0] = h1;
  ((uint32_t*)out)[1] = h2;
  ((uint32_t*)out)[2] = h3;
  ((uint32_t*)out)[3] = h4;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x64_128(const void* key, const int len, const uint32_t seed,
                         void* out) {
  const uint8_t* data = (const uint8_t*)key;
  const int nblocks = len / 16;

  std::uint64_t h1 = seed;
  std::uint64_t h2 = seed;

  const std::uint64_t c1 = static_cast<std::uint64_t>(0x87c37b91114253d5);
  const std::uint64_t c2 = static_cast<std::uint64_t>(0x4cf5ad432745937f);

  //----------
  // body

  const std::uint64_t* blocks = (const std::uint64_t*)(data);

  for (int i = 0; i < nblocks; i++) {
    std::uint64_t k1 = getblock64(blocks, i * 2 + 0);
    std::uint64_t k2 = getblock64(blocks, i * 2 + 1);

    k1 *= c1;
    k1 = rotl64(k1, 31);
    k1 *= c2;
    h1 ^= k1;

    h1 = rotl64(h1, 27);
    h1 += h2;
    h1 = h1 * 5 + 0x52dce729;

    k2 *= c2;
    k2 = rotl64(k2, 33);
    k2 *= c1;
    h2 ^= k2;

    h2 = rotl64(h2, 31);
    h2 += h1;
    h2 = h2 * 5 + 0x38495ab5;
  }

  //----------
  // tail

  const uint8_t* tail = (const uint8_t*)(data + nblocks * 16);

  std::uint64_t k1 = 0;
  std::uint64_t k2 = 0;

  switch (len & 15) {
    case 15:
      k2 ^= static_cast<std::uint64_t>(tail[14]) << 48;
    case 14:
      k2 ^= static_cast<std::uint64_t>(tail[13]) << 40;
    case 13:
      k2 ^= static_cast<std::uint64_t>(tail[12]) << 32;
    case 12:
      k2 ^= static_cast<std::uint64_t>(tail[11]) << 24;
    case 11:
      k2 ^= static_cast<std::uint64_t>(tail[10]) << 16;
    case 10:
      k2 ^= static_cast<std::uint64_t>(tail[9]) << 8;
    case 9:
      k2 ^= static_cast<std::uint64_t>(tail[8]) << 0;
      k2 *= c2;
      k2 = rotl64(k2, 33);
      k2 *= c1;
      h2 ^= k2;

    case 8:
      k1 ^= static_cast<std::uint64_t>(tail[7]) << 56;
    case 7:
      k1 ^= static_cast<std::uint64_t>(tail[6]) << 48;
    case 6:
      k1 ^= static_cast<std::uint64_t>(tail[5]) << 40;
    case 5:
      k1 ^= static_cast<std::uint64_t>(tail[4]) << 32;
    case 4:
      k1 ^= static_cast<std::uint64_t>(tail[3]) << 24;
    case 3:
      k1 ^= static_cast<std::uint64_t>(tail[2]) << 16;
    case 2:
      k1 ^= static_cast<std::uint64_t>(tail[1]) << 8;
    case 1:
      k1 ^= static_cast<std::uint64_t>(tail[0]) << 0;
      k1 *= c1;
      k1 = rotl64(k1, 31);
      k1 *= c2;
      h1 ^= k1;
  }

  //----------
  // finalization

  h1 ^= len;
  h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix64(h1);
  h2 = fmix64(h2);

  h1 += h2;
  h2 += h1;

  ((uint64_t*)out)[0] = h1;
  ((uint64_t*)out)[1] = h2;
}
*/
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_MURMUR3_HPP_INCLUDED
