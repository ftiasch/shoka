#pragma once

#include <stdexcept>

namespace ntt {

static void assert_power_of_two(int n) {
  if (n & (n - 1)) {
    throw std::invalid_argument(std::to_string(n) + " is not a power of two");
  }
}

static int min_power_of_two(int n) {
  return n == 1 ? 1 : 1 << (32 - __builtin_clz(n - 1));
}

} // namespace ntt
