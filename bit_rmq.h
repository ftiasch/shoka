#include "sparse_table.h"

#include <cstdint>

template <typename T, int N> struct BitRMQ {
public:
  void compute(int n, const T *a) {
    for (int i = 0, block_start = 0; block_start < n; ++i, block_start += 64) {
      buffer[i] =
          intra[i].compute(std::min(n - block_start, 64), a + block_start);
    }
    inter.compute((n + 63) >> 6, buffer);
  }

  T rmq(int l, int r) const {
    int lblk = l >> 6;
    int rblk = r >> 6;
    if (lblk == rblk) {
      return intra[lblk].rmq(l & 63, r & 63);
    }
    T prefix = intra[lblk].suffix[l & 63];
    T suffix = intra[rblk].prefix[r & 63];
    if (lblk + 1 <= rblk - 1) {
      return std::min({prefix, inter.rmq(lblk + 1, rblk - 1), suffix});
    }
    return std::min({prefix, suffix});
  }

private:
  static const int M = (N + 63) >> 6;

  struct Intra {
    T compute(int n, const T *a_) {
      a = a_;
      uint64_t current_stack = 0;
      T minimum;
      for (int i = 0; i < n; ++i) {
        while (current_stack && a[highest_bit(current_stack)] > a[i]) {
          current_stack ^= 1ULL << highest_bit(current_stack);
        }
        current_stack |= 1ULL << i;
        stack[i] = current_stack;
        minimum = i ? std::min(minimum, a[i]) : a[i];
        prefix[i] = minimum;
      }
      suffix[n - 1] = a[n - 1];
      for (int i = n - 1; i--;) {
        suffix[i] = std::min(suffix[i + 1], a[i]);
      }
      return minimum;
    }

    T rmq(int l, int r) const {
      return a[__builtin_ctzll(stack[r] & ~((1ULL << l) - 1))];
    }

    T prefix[64], suffix[64];

  private:
    static int highest_bit(uint64_t mask) { return 63 - __builtin_clzll(mask); }

    const T *a;
    uint64_t stack[64];
  } intra[M];
  T buffer[M];
  SparseTable<T, M> inter;
};
