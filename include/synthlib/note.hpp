#pragma once

#include <cstdint>

namespace synthlib {

enum class Note : std::uint8_t {
  C = 0,
  Csh = 1,
  Db = 1,
  D = 2,
  Dsh = 3,
  Eb = 3,
  E = 4,
  F = 5,
  Fsh = 6,
  Gb = 6,
  G = 7,
  Gsh = 8,
  Ab = 8,
  A = 9,
  Ash = 10,
  Bb = 10,
  B = 11
};

} // namespace synthlib
