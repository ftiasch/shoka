#pragma once

#include <concepts>

template <std::integral T> static inline constexpr T is_prime(T n) {
  if (n < 2) {
    return false;
  }
  for (T d = 2; d * d <= n; ++d) {
    if (n % d == 0) {
      return false;
    }
  }
  return true;
}
