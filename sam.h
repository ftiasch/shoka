#include "empty_class.h"

#include <array>
#include <vector>

template <int C, typename BaseNode = EmptyClass> struct SamT {
  struct Node : public BaseNode {
    explicit Node(int length_ = 0) : length{length_}, go{} {}

    int length;
    Node *parent = nullptr;
    std::array<Node *, C> go;
  };

  explicit SamT(int n) : node_count(1), nodes(n << 1) {}

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
        nq->go = q->go;
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
