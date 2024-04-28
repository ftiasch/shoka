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
    for (int i = n - (1 << k); i < n; i++) {
      new_order[count[rank[i]]++] = i;
    }
    for (int j : order) {
      int i = j - (1 << k);
      if (i >= 0) {
        new_order[count[rank[i]]++] = i;
      }
    }
    order.swap(new_order);
    int cur{-1};
    std::pair<int, int> lsp{-1, -1};
    for (int j = 0; j < n; j++) {
      int i = order[j];
      std::pair<int, int> p{rank[i],
                            i + (1 << k) < n ? rank[i + (1 << k)] : -1};
      if (lsp != p) {
        count[++cur] = j;
        lsp = p;
      }
      new_rank[i] = cur;
    }
    rank.swap(new_rank);
  }
  return {std::move(order), std::move(rank)};
}
