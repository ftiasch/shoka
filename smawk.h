#include "types/total_monotone.h"

#include <numeric>
#include <vector>

template <IsTM A> struct SMAWK {
  using E = typename A::E;
  using EI = std::pair<E, int>;
  using Result = std::vector<EI>;

  void operator()(Result &row_min, const A &a, int n, int m) {
    stack.resize(n);
    cols.resize(m + n + n);
    std::iota(cols.begin(), cols.begin() + m, 0);
    row_min.resize(n);
    recur(row_min, a, n, m, 0, 0, m);
  }

  Result operator()(const A &a, int n, int m) {
    Result row_min;
    operator()(row_min, a, n, m);
    return row_min;
  }

  void recur(Result &row_min, const A &a, int n, int m, int k, int begin,
             int end) {
    if (n < (2 << k)) {
      auto r = (1 << k) - 1;
      auto &ref = row_min[r] = query(a, r, cols[begin]);
      for (int i = begin + 1; i < end; i++) {
        ref = std::min(ref, query(a, r, cols[i]));
      }
    } else {
      int top{0};
      for (int i = begin; i < end; i++) {
        auto c = cols[i];
        while (top && stack[top - 1] > a((top << k) - 1, c)) {
          top--;
        }
        auto r = ((top + 1) << k) - 1;
        if (r < n) {
          cols[end + top] = c;
          stack[top++] = a(r, c);
        }
      }
      begin = end, end += top;
      recur(row_min, a, n, m, k + 1, begin, end);
      auto offset = 1 << k;
      for (int r = offset - 1, p = begin; r < n; r += offset << 1) {
        auto high = r + offset < n ? row_min[r + offset].second + 1 : m;
        auto &ref = row_min[r] = query(a, r, cols[p]);
        while (p + 1 < end && cols[p + 1] < high) {
          ref = std::min(ref, query(a, r, cols[++p]));
        }
      }
    }
  }

  EI query(const A &a, int r, int c) { return {a(r, c), c}; }

  std::vector<E> stack;
  std::vector<int> cols;
};
