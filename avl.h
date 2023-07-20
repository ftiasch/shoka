#include "free_queue.h"

#include <cassert>
#include <vector>

template <typename NodeBase, typename Impl> struct AVLTreeBase {
  struct Node : public NodeBase {
    int height;
    size_t size;
    Node *left, *right;
  };

  explicit AVLTreeBase(int n_) : nodes(n_ << 1), free_queue(n_ << 1) {
    assert(free_queue.allocate() == 0);
    auto n = null_node();
    n->height = 0;
    n->size = 0;
    n->left = n->right = null_node();
  }

  Node *null_node() { return nodes.data(); }

  Node *new_leaf() {
    auto n = new_node();
    n->height = 0;
    n->size = 1;
    n->left = n->right = null_node();
    return n;
  }

  Node *merge(Node *u, Node *v) {
    if (u == null_node()) {
      return v;
    }
    if (v == null_node()) {
      return u;
    }
    if (std::abs(u->height - v->height) <= 1) {
      return new_node(u, v);
    }
    Impl::propagate(v);
    if (u->height + 2 == v->height && v->right->height + 2 == v->height) {
      auto [vl, vr] = destruct(v);
      auto [vll, vlr] = destruct(vl);
      return new_node(new_node(u, vll), new_node(vlr, vr));
    }
    if (u->height < v->height) {
      auto [vl, vr] = destruct(v);
      return merge(merge(u, vl), vr);
    } else {
      auto [ul, ur] = destruct(u);
      return merge(ul, merge(ur, v));
    }
  }

  std::pair<Node *, Node *> split(Node *u, size_t k) {
    if (k == 0) {
      return {null_node(), u};
    }
    if (k == u->size) {
      return {u, null_node()};
    }
    auto [ul, ur] = destruct(u);
    if (k <= ul->size) {
      auto [a, b] = split(ul, k);
      return {a, merge(b, ur)};
    } else {
      auto [a, b] = split(ur, k - ul->size);
      return {merge(ul, a), b};
    }
  }

private:
  Node *collect(Node *n) {
    n->height = std::max(n->left->height, n->right->height) + 1;
    n->size = n->left->size + n->right->size;
    Impl::collect(n);
    return n;
  }

  Node *new_node() { return new (nodes.data() + free_queue.allocate()) Node(); }

  Node *new_node(Node *l, Node *r) {
    auto n = new_node();
    n->left = l;
    n->right = r;
    return collect(n);
  }

  std::pair<Node *, Node *> destruct(Node *u) {
    Impl::propagate(u);
    free_node(u);
    return {u->left, u->right};
  }

  void free_node(Node *n) { free_queue.free(n - nodes.data()); }

  std::vector<Node> nodes;
  FreeQueue free_queue;
};
