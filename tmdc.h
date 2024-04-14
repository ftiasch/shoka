#include "types/total_monotone.h"

#include <vector>

namespace tmdc_details {

template <IsTM A> using EI = std::pair<typename A::E, int>;

template <IsTM A>
void recur(std::vector<EI<A>> &row_min, const A &a, int l, int r, int low,
           int high) {
  using E = typename A::E;
  if (l < r) {
    auto m = (l + r) >> 1;
    auto &best = row_min[m] = {a(m, low), low};
    for (int k = low + 1; k < high; k++) {
      best = std::min(best, {a(m, k), k});
    }
    recur(row_min, a, l, m, low, best.second + 1);
    recur(row_min, a, m + 1, r, best.second, high);
  }
}
} // namespace tmdc_details

template <IsTM A>
void tmdc(std::vector<tmdc_details::EI<A>> &row_min, const A &a, int n, int m) {
  row_min.resize(n);
  tmdc_details::recur(row_min, a, 0, n, 0, m);
}
