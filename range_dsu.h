#include "dsu.h"

#include <tuple>
#include <utility>
#include <vector>

static inline Dsu
range_dsu(int n, const std::vector<std::tuple<int, int, int>> &queries) {
  std::vector<int> count(n + 1);
  int Q = queries.size();
  for (auto [x, y, l] : queries) {
    count[l]++;
  }
  for (int i = 1; i <= n; i++) {
    count[i] += count[i - 1];
  }
  std::vector<std::pair<int, int>> queue(Q);
  for (auto [x, y, l] : queries) {
    queue[--count[l]] = {x, y};
  }
  Dsu dsu(n);
  int rear{Q};
  for (int l = n; l >= 1; l--) {
    auto head = count[l];
    for (int i = rear; i-- > head;) {
      auto [x, y] = queue[i];
      if (dsu.merge(x, y)) {
        queue[i++] = queue[head];
        queue[head++] = {
            x + 1,
            y + 1,
        };
      }
    }
    rear = head;
  }
  return dsu;
}
