#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

template <typename Mod> std::vector<int> egz(const std::vector<int> &a) {
  int n = a.size();
  assert(n & 1);
  auto p = (n + 1) >> 1;
  Mod::set_mod(p);
  std::vector<std::pair<int, int>> sorted_a(n);
  for (int i = 0; i < n; ++i) {
    sorted_a[i] = {a[i] % p, i};
  }
  std::sort(sorted_a.begin(), sorted_a.end());
  for (int i = 0; i + p - 1 < n; ++i) {
    if (sorted_a[i].first == sorted_a[i + p - 1].first) {
      std::vector<int> plan(p);
      for (int j = 0; j < p; ++j) {
        plan[j] = sorted_a[i + j].second;
      }
      return plan;
    }
  }
  auto offset = -Mod{sorted_a[0].first};
  std::vector<int> from(p, -1);
  from[0] = 0;
  int last = p - 1;
  for (int i = 1; i + p - 1 < n; ++i) {
    offset -= Mod{sorted_a[i].first};
    Mod d{sorted_a[i + p - 1].first - sorted_a[i].first};
    while (~last && ~from[last]) {
      last--;
    }
    assert(~last);
    int low = 0, high = (Mod{last} * d.inv()).get();
    while (low + 1 < high) {
      int middle = (low + high) >> 1;
      if (~from[(Mod{middle} * d).get()]) {
        low = middle;
      } else {
        high = middle;
      }
    }
    auto x = Mod{low} * d;
    assert(~from[x.get()]);
    assert(!~from[(x + d).get()]);
    from[(x + d).get()] = i;
  }
  std::vector<bool> picked(p);
  while (offset.get()) {
    int i = from[offset.get()];
    picked[i] = true;
    offset -= Mod{sorted_a[i + p - 1].first - sorted_a[i].first};
  }
  std::vector<int> plan{sorted_a[0].second};
  plan.reserve(p);
  for (int i = 1; i + p - 1 < n; ++i) {
    plan.push_back(sorted_a[picked[i] ? i + p - 1 : i].second);
  }
  return plan;
}
