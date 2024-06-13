#include <numeric>
#include <vector>

class PathCompressionDsu {
  std::vector<int> parent;

public:
  explicit PathCompressionDsu(int n) : parent(n) {
    std::iota(parent.begin(), parent.end(), 0);
  }

  int find(int u) {
    if (parent[u] != u) {
      parent[u] = find(parent[u]);
    }
    return parent[u];
  }

  bool merge(int a, int b) {
    if (find(a) == find(b)) {
      return false;
    }
    parent[find(a)] = find(b);
    return true;
  }
};
