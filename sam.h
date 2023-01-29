#include <cstring>
#include <vector>

namespace sam {

struct EmptyNode {};

} // namespace sam

template <int C, typename BaseNode = sam::EmptyNode> struct SAM {
  struct Node : BaseNode {
    Node(int length_ = 0) : length(length_), parent(nullptr) {
      memset(go, 0, sizeof(go));
    }

    int length;
    Node *parent, *go[C];
  };

  SAM(int n) : node_count(1), nodes(n << 1) {}

  Node *root() { return nodes.data(); }

  Node *extend(Node *p, int c) {
    Node *np = new (nodes.data() + (node_count++)) Node(p->length + 1);
    while (p && !p->go[c]) {
      p->go[c] = np;
      p = p->parent;
    }
    if (!p) {
      np->parent = root();
    } else {
      Node *q = p->go[c];
      if (p->length + 1 == q->length) {
        np->parent = q;
      } else {
        Node *nq = new (nodes.data() + (node_count++)) Node(p->length + 1);
        memcpy(nq->go, q->go, sizeof(q->go));
        nq->parent = q->parent;
        np->parent = q->parent = nq;
        while (p && p->go[c] == q) {
          p->go[c] = nq;
          p = p->parent;
        }
      }
    }
    return np;
  }

protected:
  int node_count;
  std::vector<Node> nodes;
};
