#include <algorithm>
#include <array>
#include <queue>

namespace trie {
struct EmptyNode {};
} // namespace trie

template <int C, typename BaseNode = trie::EmptyNode> struct TrieT {
  struct Node : public BaseNode {
    Node() { std::fill(go.begin(), go.end(), nullptr); }

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
