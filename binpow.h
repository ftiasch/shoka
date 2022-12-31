#pragma once

#include <cstdint>
#include <type_traits>

template <typename T, typename N = uint64_t>
static constexpr T binpow(T a, N n) {
  static_assert(std::is_integral_v<N>);
  auto result = T::mul_id();
  while (n) {
    if (n & 1) {
      result *= a;
    }
    a *= a;
    n >>= 1;
  }
  return result;
}
