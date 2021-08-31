#include <cstring>

template <int N> class StaticDSU {
public:
  void init(int n) { memset(parent, -1, sizeof(*parent) * n); }

  int find(int u) {
    if (!~parent[u]) {
      return u;
    }
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

private:
  int parent[N];
};
