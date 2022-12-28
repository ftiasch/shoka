#pragma once

#include <cstdint>
#include <type_traits>

template <typename T, typename N = uint64_t>
static constexpr std::enable_if_t<std::is_integral_v<N>, T> binpow(T a, N n) {
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
