#pragma once

#include <cstdint>

struct uint128_shim {
  static uint128_shim multiply(uint64_t x, uint64_t y) {
#ifdef SHOKA_UINT128_ASM
    uint64_t high, low;
    __asm__("mulq %3\n\t" : "=d"(high), "=a"(low) : "%a"(x), "rm"(y) : "cc");
    return {high, low};
#else
    uint64_t x_lo = x & UINT32_MAX;
    uint64_t x_hi = x >> 32;
    uint64_t y_lo = y & UINT32_MAX;
    uint64_t y_hi = y >> 32;
    uint64_t lo_lo = x_lo * y_lo;
    uint64_t hi_lo = x_hi * y_lo;
    uint64_t lo_hi = x_lo * y_hi;
    uint64_t cy = (hi_lo & UINT32_MAX) + (lo_hi & UINT32_MAX) + (lo_lo >> 32);
    return {x_hi * y_hi + (hi_lo >> 32) + (lo_hi >> 32) + (cy >> 32),
            (cy & UINT32_MAX) << 32 | (lo_lo & UINT32_MAX)};
#endif
  }

  uint64_t high, low;
};
