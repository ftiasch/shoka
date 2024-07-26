#include "binom.h"
#include "poly_conv.h"

#include <algorithm>
#include <span>
#include <vector>

/*
 * Compute R[j] = sum_i U[i] * W(i, j) for j = 0, ..., m - 1.
 * Here, W(i, j) is the number of north-easy walk from (i, 0) to (n - 1, j)
 * where y < A[x] holds
 */
template <typename Mod> struct NorthEastLatticePath {
  explicit NorthEastLatticePath(const std::vector<int> &a_,
                                const std::vector<Mod> &U_, int m)
      : a{a_}, U{U_}, R(m), binom(a.size() + m) {
    recur(0, a_.size(), 0, m);
  }

  const std::vector<Mod> &result() const { return R; }

private:
  void recur(int l, int r, int lo, int hi) {
    // Invariance: U[lo, high) = 0
    if (l + 1 == r) {
      for (int j = lo; j < hi && j < a[l]; j++) {
        R[j] = U[l];
      }
    } else {
      int m = (l + r) >> 1;
      int mi = a[m];
      if (lo < mi) {
        recur(l, m, lo, mi);
        rect(std::span{U.begin() + m, U.begin() + r},
             std::span(R.begin() + lo, R.begin() + mi));
      }
      if (mi < hi) {
        recur(m, r, mi, hi);
      }
    }
  }

  void rect(std::span<Mod> U, std::span<Mod> R) {
    tmp_U.assign(U.begin(), U.end());
    tmp_R.assign(R.begin(), R.end());
    std::ranges::fill(U, Mod{0});
    std::ranges::fill(R, Mod{0});
    rect_adj(tmp_U, R);
    rect_adj(tmp_R, U);
    rect_op(tmp_U, U, R.size());
    rect_op(tmp_R, R, U.size());
  }

  void rect_adj(const std::vector<Mod> &U, std::span<Mod> R) {
    int n = U.size();
    int m = R.size();
    lhs.resize(n);
    for (int i = 0; i < n; i++) {
      lhs[i] = U[i] * binom.inv_fact[n - 1 - i];
    }
    rhs.resize(n + m - 1);
    for (int i = 0; i < n + m - 1; i++) {
      rhs[i] = binom.fact[i];
    }
    conv(out, lhs, rhs, n + m);
    for (int i = 0; i < m; i++) {
      R[i] += out[n - 1 + i] * binom.inv_fact[i];
    }
  }

  void rect_op(const std::vector<Mod> &U, std::span<Mod> UU, int h) {
    int n = U.size();
    rhs.resize(n);
    for (int i = 0; i < n; i++) {
      rhs[i] = binom.fact[h - 1 + i] * binom.inv_fact[i];
    }
    conv(out, U, rhs);
    for (int i = 0; i < n; i++) {
      UU[i] += out[i] * binom.inv_fact[h - 1];
    }
  }

  const std::vector<int> &a;
  std::vector<Mod> U, R, tmp_U, tmp_R, lhs, rhs, out;
  Binom<Mod> binom;
  PolyConv<Mod> conv;
};
