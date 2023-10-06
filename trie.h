#include "empty_class.h"

#include <algorithm>
#include <array>
#include <queue>
#include <ranges>

template <int C, typename BaseNode = EmptyClass> struct TrieT {
  struct Node : public BaseNode {
    Node() { std::ranges::fill(go, nullptr); }

    std::array<Node *, C> go;
  };

  explicit TrieT(int n) : node_count{1}, nodes(1 + n) {}

  Node *root() { return nodes.data(); }

  Node *extend(Node *p, int c) {
    if (p->go[c] == nullptr) {
      p->go[c] = new (nodes.data() + (node_count++)) Node();
    }
    return p->go[c];
  }

protected:
  int node_count;
  std::vector<Node> nodes;
};
