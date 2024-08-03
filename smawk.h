#include "types/matrix_2d.h"

#include <numeric>
#include <vector>

template <Is2dMatrix A> class SMAWK {
  using E = typename A::E;

  void recur(const A &a, int n, int m, int k, int begin, int end) {
    if (n < (2 << k)) {
      auto r = (1 << k) - 1;
      auto ref = query(a, r, cols[begin]);
      for (int i = begin + 1; i < end; i++) {
        ref = std::min(ref, query(a, r, cols[i]));
      }
      std::tie(row_min[r], row_argmin[r]) = ref;
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
      recur(a, n, m, k + 1, begin, end);
      auto offset = 1 << k;
      for (int r = offset - 1, p = begin; r < n; r += offset << 1) {
        auto high = r + offset < n ? row_argmin[r + offset] + 1 : m;
        auto ref = query(a, r, cols[p]);
        while (p + 1 < end && cols[p + 1] < high) {
          ref = std::min(ref, query(a, r, cols[++p]));
        }
        std::tie(row_min[r], row_argmin[r]) = ref;
      }
    }
  }

  std::pair<E, int> query(const A &a, int r, int c) const {
    return {a(r, c), c};
  }

  std::vector<int> cols, row_argmin;
  std::vector<E> row_min, stack;

public:
  auto const &operator()(const A &a, int n, int m) {
    cols.resize(m + n + n);
    std::iota(cols.begin(), cols.begin() + m, 0);
    row_min.resize(n);
    row_argmin.resize(n);
    stack.resize(n);
    recur(a, n, m, 0, 0, m);
    return row_min;
  }
};
