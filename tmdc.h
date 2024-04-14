#include "types/total_monotone.h"

#include <vector>

template <IsTM A> struct TMDC {
  using E = typename A::E;
  using Result = std::vector<E>;

  Result operator()(const A &a, int n, int m) {
    Result row_min(n);
    recur(row_min, a, 0, n, 0, m);
    return row_min;
  }

  void recur(Result &row_min, const A &a, int l, int r, int low, int high) {
    if (l < r) {
      auto m = (l + r) >> 1;
      std::pair<E, int> best = {a(m, low), low};
      for (int k = low + 1; k < high; k++) {
        best = std::min(best, {a(m, k), k});
      }
      row_min[m] = best.first;
      recur(row_min, a, l, m, low, best.second + 1);
      recur(row_min, a, m + 1, r, best.second, high);
    }
  }
};
