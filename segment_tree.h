#include <memory>
#include <vector>

template <typename Node, typename Derived> struct SegmentTreeBase {
  SegmentTreeBase(int n_) : n(n_), nodes(n << 1) {}

  template <typename Handler> Handler traverse(Handler &&h) {
    return traverse<Handler, true>(std::move(h), 0, n - 1, 0, 0);
  }

  template <typename Handler> Handler traverse(Handler &&h, int a, int b) {
    return traverse<Handler, false>(std::move(h), 0, n - 1, a, b);
  }

private:
  Node &get_node(int l, int r) { return nodes[l + r | (l != r)]; }

  template <typename Handler, bool all>
  Handler traverse(Handler &&h, int l, int r, int a, int b) {
    Node &n = get_node(l, r);
    if (all) {
      h.update(l, r, n);
      if (l == r) {
        return h;
      }
    } else {
      if (b < l || r < a) {
        return h;
      }
      if (a <= l && r <= b) {
        h.update(l, r, n);
        return h;
      }
    }
    int m = (l + r) >> 1;
    auto &ln = get_node(l, m);
    auto &rn = get_node(m + 1, r);
    Derived::propagate(l, m, r, n, ln, rn);
    auto h1 = traverse<Handler, all>(std::move(h), l, m, a, b);
    auto h2 = traverse<Handler, all>(std::move(h1), m + 1, r, a, b);
    Derived::collect(l, m, r, n, ln, rn);
    return h2;
  }

  int n;
  std::vector<Node> nodes;
};

/*
struct Node {
  // TODO
};

struct SegmentTree : public SegmentTreeBase<Node, SegmentTree> {
  struct Build {
    void update(int l, int r, Node &n) {
      // TODO
    }

    const std::vector<int> &a;
  };

  struct Query {
    void update(int l, int r, Node &n) const {
      // TODO
    }
  };

  SegmentTree(const std::vector<int> &a) : SegmentTreeBase(a.size()) {
    traverse(Build{a});
  }

  static void propagate(int l, int m, int r, Node &n, Node &ln, Node &rn) {
    // TODO
  }

  static void collect(int l, int m, int r, Node &n, Node &ln, Node &rn) {
    // TODO
  }
};
*/
