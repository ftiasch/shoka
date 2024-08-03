#include "types/matrix_2d.h"

#include <vector>

template <Is2dMatrix A> class TMDC {
  void recur(const A &a, int l, int r, int low, int high) {
    if (l < r) {
      int m = (l + r) >> 1;
      auto best = std::make_pair(a(m, low), low);
      for (int k = low + 1; k < high; k++) {
        best = std::min(best, {a(m, k), k});
      }
      row_min[m] = best.first;
      recur(a, l, m, low, best.second + 1);
      recur(a, m + 1, r, best.second, high);
    }
  }

  std::vector<typename A::E> row_min;

public:
  auto const &operator()(const A &a, int n, int m) {
    row_min.resize(n);
    recur(a, 0, n, 0, m);
    return row_min;
  }
};
