#include <algorithm>
#include <ranges>
#include <vector>

// Assume s[i] in [0, n)
static inline std::pair<std::vector<int>, std::vector<int>>
suffix_array(const std::vector<int> &s) {
  int n = s.size();
  std::vector<int> count(n), rank(s.begin(), s.end()), order(n), new_order(n),
      new_rank(n);
  for (int i = 0; i < n; i++) {
    count[s[i]]++;
  }
  for (int i = 1; i < n; i++) {
    count[i] += count[i - 1];
  }
  for (int i = n; i--;) {
    order[--count[s[i]]] = i;
  }
  for (int k = 0; 1 << k < n; k++) {
    std::ranges::fill(count, 0);
    for (int i = 0; i < n; i++) {
      count[rank[i]]++;
    }
    for (int i = 1; i < n; i++) {
      count[i] += count[i - 1];
    }
    for (int i_ : order | std::views::reverse) {
      int i = i_ - (1 << k);
      if (i >= 0) {
        new_order[--count[rank[i]]] = i;
      }
    }
    for (int i = n; i-- > n - (1 << k);) {
      new_order[--count[rank[i]]] = i;
    }
    order.swap(new_order);
    int cur{-1};
    std::pair<int, int> last_p{-1, -1};
    for (int i : order) {
      std::pair<int, int> p{rank[i],
                            i + (1 << k) < n ? rank[i + (1 << k)] : -1};
      new_rank[i] = (cur += last_p != p);
      last_p = p;
    }
    rank.swap(new_rank);
  }
  return {std::move(order), std::move(rank)};
}
