#pragma once

#include <cstdint>

// a^{-1} mod m
static inline constexpr uint32_t euclidean_inv(uint32_t a, uint32_t m) {
  return a == 1 ? 1
                : (1 + static_cast<uint64_t>(a - euclidean_inv(m % a, a)) *
                           static_cast<uint64_t>(m)) /
                      a;
}
