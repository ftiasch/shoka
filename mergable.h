#pragma once

#include <cstdint>
#include <random>
#include <utility>
#include <vector>

namespace details {

struct MergableNodePointer {
  explicit MergableNodePointer(int p_ = 0) : p(p_) {}

  explicit operator int() const { return p; }

  int unwrap() const { return p; }

  int p;
};

template <typename NodeBase> struct MergableNodeT : public NodeBase {
  int size;
  MergableNodePointer left_child, right_child;
  uint32_t rank;
};

} // namespace details

template <typename NodeBase, typename Impl> struct MergableTreapBase {
  using Node = details::MergableNodeT<NodeBase>;
  using Pointer = details::MergableNodePointer;

  explicit MergableTreapBase(int n_) : number_of_node(1), nodes(n_) {
    std::mt19937 gen;
    for (auto &n : nodes) {
      n.rank = gen();
    }
    Node *n = nodes.data();
    n->rank = std::numeric_limits<uint32_t>::max();
    n->size = 0;
    n->left_child = n->right_child = Pointer(0);
  }

  static Pointer null_pointer() { return Pointer(0); }

  Pointer new_node() {
    Pointer p(number_of_node++);
    Node *n = get_node(p);
    n->size = 1;
    n->left_child = n->right_child = null_pointer();
    return p;
  }

  Node *get_node(Pointer p) { return nodes.data() + p.unwrap(); }

  std::pair<Pointer, Pointer> split(Pointer pn, int k) {
    if (k == 0) {
      return {Pointer(0), pn};
    }
    Node *n = get_node(pn);
    if (k == n->size) {
      return {pn, Pointer(0)};
    }
    Impl::propagate(n, get_node(n->left_child), get_node(n->right_child));
    Node *ln = get_node(n->left_child);
    Node *rn = get_node(n->right_child);
    if (k <= ln->size) {
      auto result = split(n->left_child, k);
      collect(n, get_node(n->left_child = result.second), rn);
      return {result.first, pn};
    } else {
      auto result = split(n->right_child, k - 1 - ln->size);
      collect(n, ln, get_node(n->right_child = result.first));
      return {pn, result.second};
    }
  }

  Pointer merge(Pointer pu, Pointer pv) {
    if (pu.unwrap() == 0) {
      return pv;
    }
    if (pv.unwrap() == 0) {
      return pu;
    }
    Node *u = get_node(pu);
    Node *v = get_node(pv);
    if (u->rank < v->rank) {
      Impl::propagate(u, get_node(u->left_child), get_node(u->right_child));
      u->right_child = merge(u->right_child, pv);
      collect(u, get_node(u->left_child), get_node(u->right_child));
      return pu;
    } else {
      Impl::propagate(v, get_node(v->left_child), get_node(v->right_child));
      v->left_child = merge(pu, v->left_child);
      collect(v, get_node(v->left_child), get_node(v->right_child));
      return pv;
    }
  }

private:
  Pointer get_pointer(const Node *n) const {
    return NodePonter(n - nodes.data());
  }

  static void collect(Node *n, const Node *ln, const Node *rn) {
    Impl::collect(n, ln, rn);
    n->size = ln->size + 1 + rn->size;
  }

  int number_of_node;
  std::vector<Node> nodes;
};
