#include <cstring>

template <int N, int C> struct SAM {
  struct Node {
    int length;
    Node *parent, *go[C];
  };

  SAM() { memset(nodes, 0, sizeof(nodes)); }

  Node *root() { return nodes; }

  void reset() {
    memset(nodes, 0, sizeof(*nodes) * (node_count));
    node_count = 1;
  }

  Node *extend(Node *p, int c) {
    Node *np = new_node(p->length + 1);
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
        Node *nq = new_node(p->length + 1);
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

  int node_count = 1;
  Node nodes[N << 1];

private:
  Node *new_node(int length) { return nodes + (node_count++); }
};
