#pragma once

#include "sparse_table.h"

#include <cstdint>

template <typename T_, T_ id, typename Compare = std::less<T_>> struct BitRmqT {
  using T = T_;
  using Semilattice = SemilatticeT<T, id, Compare>;

  explicit BitRmqT(std::vector<T> &&values_)
      : n{static_cast<int>(values_.size())}, m{block_id(n - 1) + 1},
        values{std::move(values_)},
        block_meet{compute_block_meet(n, m, values)}, inter{block_meet} {
    block.reserve(m);
    for (int i = 0, s = 0; i < m; ++i, s += 64) {
      block.emplace_back(values.data() + s, std::min(n - s, 64));
    }
  }

  T operator()(int l, int r) const {
    if (l == r) {
      return Semilattice::id;
    }
    r--;
    int lb = block_id(l);
    int rb = block_id(r);
    if (lb == rb) {
      return block[lb](l & 63, r & 63);
    }
    T pre_suf =
        Semilattice::meet(block[lb].suffix[l & 63], block[rb].prefix[r & 63]);
    return (lb + 1 <= rb - 1)
               ? Semilattice::meet(pre_suf, inter(lb + 1, rb - 1))
               : pre_suf;
  }

private:
  static constexpr int block_id(int index) { return index >> 6; }

  struct Block {
    explicit Block(const T *a_, int n) {
      a = a_;
      uint64_t current_stack = 0;
      T minimum = Semilattice::id;
      for (int i = 0; i < n; ++i) {
        while (current_stack &&
               Compare{}(a[i], a[highest_bit(current_stack)])) {
          current_stack ^= 1ULL << highest_bit(current_stack);
        }
        current_stack |= 1ULL << i;
        stack[i] = current_stack;
        minimum = Semilattice::meet(minimum, a[i]);
        prefix[i] = minimum;
      }
      suffix[n - 1] = a[n - 1];
      for (int i = n - 1; i--;) {
        suffix[i] = Semilattice::meet(suffix[i + 1], a[i]);
      }
    }

    T operator()(int l, int r) const {
      return a[__builtin_ctzll(stack[r] & ~((1ULL << l) - 1))];
    }

    std::array<T, 64> prefix, suffix;

  private:
    static int highest_bit(uint64_t mask) { return 63 - __builtin_clzll(mask); }

    const T *a;

    std::array<uint64_t, 64> stack;
  };

  static std::vector<T> compute_block_meet(int n, int m,
                                           const std::vector<T> &values) {
    std::vector<T> meet(m, Semilattice::id);
    for (int i = 0; i < n; ++i) {
      meet[block_id(i)] = Semilattice::meet(meet[block_id(i)], values[i]);
    }
    return meet;
  }

  int n, m;
  const std::vector<T> values;
  std::vector<T> block_meet;
  SparseTableT<Semilattice> inter;
  std::vector<Block> block;
};
