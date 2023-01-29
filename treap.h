#include <cstdint>
#include <random>
#include <utility>
#include <vector>

namespace treap {

template <typename NodeBase> struct AANodeT : public NodeBase {
  int size;
  AANodeT *left_child, *right_child;
  uint32_t rank;
};

} // namespace treap

template <typename NodeBase, typename Impl> struct AATreeBase {
  using Node = AANodeT::AANodeT<NodeBase>;

  explicit AATreeBase(int n_) : number_of_node(1), nodes(n_) {
    std::mt19937 gen;
    for (auto &n : nodes) {
      n.rank = gen();
    }
    Node *n = null_node();
    n->rank = std::numeric_limits<uint32_t>::max();
    n->size = 0;
    n->left_child = n->right_child = null_node();
  }

  Node *null_node() { return nodes.data(); }

  Node *new_node() {
    Node *n = nodes.data() + (number_of_node++);
    n->size = 1;
    n->left_child = n->right_child = null_node();
    return n;
  }

  std::pair<Node *, Node *> split(Node *n, int k) {
    if (k == 0) {
      return {null_node(), n};
    }
    if (k == n->size) {
      return {n, null_node()};
    }
    Impl::propagate(n);
    Node *ln = n->left_child;
    Node *rn = n->right_child;
    if (k <= ln->size) {
      auto result = split(n->left_child, k);
      n->left_child = result.second;
      return {result.first, collect(n)};
    } else {
      auto result = split(n->right_child, k - 1 - ln->size);
      n->right_child = result.first;
      return {collect(n), result.second};
    }
  }

  Node *merge(Node *u, Node *v) {
    if (u == null_node()) {
      return v;
    }
    if (v == null_node()) {
      return u;
    }
    if (u->rank < v->rank) {
      Impl::propagate(u);
      u->right_child = merge(u->right_child, v);
      return collect(u);
    } else {
      Impl::propagate(v);
      v->left_child = merge(u, v->left_child);
      return collect(v);
    }
  }

private:
  static Node *collect(Node *n) {
    Impl::collect(n);
    n->size = n->left_child->size + 1 + n->right_child->size;
    return n;
  }

  int number_of_node;
  std::vector<Node> nodes;
};
