#pragma once

#include <type_traits>

template <typename T> static constexpr T is_prime(T n) {
  static_assert(std::is_integral_v<T>);
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
