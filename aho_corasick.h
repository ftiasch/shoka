#include "empty_class.h"

#include <algorithm>
#include <array>
#include <queue>

template <int C, typename BaseNode = EmptyClass> struct AhoCorasick {
  struct Node : public BaseNode {
    Node() { std::fill(go.begin(), go.end(), nullptr); }

    Node *fail;
    std::array<Node *, C> go;
  };

  explicit AhoCorasick(int n) : node_count{1}, nodes(1 + n) {}

  Node *root() { return nodes.data(); }

  Node *extend(Node *p, int c) {
    if (p->go[c] == nullptr) {
      p->go[c] = new (nodes.data() + (node_count++)) Node();
    }
    return p->go[c];
  }

  auto build() {
    std::vector<Node *> queue;
    auto r = root();
    for (int c = 0; c < C; c++) {
      auto &v = r->go[c];
      if (v != nullptr) {
        queue.push_back(v);
      }
      (v != nullptr ? v->fail : v) = r;
    }
    for (int hd = 0; hd < queue.size(); hd++) {
      auto u = queue[hd];
      for (int c = 0; c < C; c++) {
        auto &v = u->go[c];
        if (v != nullptr) {
          queue.push_back(v);
        }
        (v != nullptr ? v->fail : v) = u->fail->go[c];
      }
    }
    return queue;
  }

protected:
  int node_count;
  std::vector<Node> nodes;
};
