#include <algorithm>
#include <array>
#include <queue>

namespace ac {

struct EmptyNode {};

} // namespace ac

template <int C, typename BaseNode = ac::EmptyNode> struct AhoCorasick {
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

  void build() {
    std::queue<Node *> queue;
    queue.push(root());
    while (!queue.empty()) {
      auto u = queue.front();
      queue.pop();
      for (int c = 0; c < C; ++c) {
        auto &v = u->go[c];
        if (v != nullptr) {
          queue.push(v);
        }
        (v != nullptr ? v->fail : v) = u->fail->go[c];
      }
    }
  }

protected:
  int node_count;
  std::vector<Node> nodes;
};
