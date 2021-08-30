#include "testlib.h"

#include <vector>

std::vector<int> rand_tree(int n, int r) {
  std::vector<int> parent(n, -1);
  int y = r;
  for (int _ = 1; _ < n; ++_) {
    int x = rnd.next(0, n - 1);
    while (x == r || ~parent[x]) {
      y = x;
      x = rnd.next(0, n - 1);
    }
    parent[x] = y;
    y = x;
  }
  return parent;
}

static std::string rand_parenthesis(int n) {
  std::vector<bool> bits((n << 1) + 1);
  for (int i = 0; i <= n; ++i) {
    bits[i] = 1;
  }
  shuffle(bits.begin(), bits.end());
  int sum = 0;
  std::pair<int, int> best{0, 0};
  for (int i = 0; i <= n << 1; ++i) {
    sum += bits[i] ? -1 : 1;
    best = std::min(best, std::make_pair(sum, i + 1));
  }
  std::string result;
  for (int i = 0; i < n << 1; ++i) {
    result += bits[(best.second + i) % ((n << 1) + 1)] ? ')' : '(';
  }
  return result;
}
