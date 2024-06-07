#pragma once

#include <bit>
#include <concepts>
#include <limits>

template <std::integral Int> static inline constexpr Int min_pow_of_two(Int n) {
  using UInt = std::make_unsigned_t<Int>;
  return static_cast<UInt>(1) << (std::numeric_limits<UInt>::digits -
                                  std::countl_zero(static_cast<UInt>(n)));
}
