#include <vector>

class Dsu {
  struct Node {
    int p, r;
  };

  std::vector<Node> node;

public:
  explicit Dsu(int n) : node(n) {
    for (int i = 0; i < n; i++) {
      node[i] = {i, 0};
    }
  }

  int find(int u) {
    while (node[u].p != u) {
      u = node[u].p = node[node[u].p].p;
    }
    return u;
  }

  bool merge(int a, int b) {
    a = find(a), b = find(b);
    if (a == b) {
      return false;
    }
    if (node[a].r < node[b].r) {
      std::swap(a, b);
    }
    node[b].p = a;
    node[a].r += node[a].r == node[b].r;
    return true;
  }
};
