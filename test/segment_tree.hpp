#include "segment_tree.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

using u32 = uint32_t;

struct Node {
  u32 tag, sum;
};

struct Build {
  void update(int l, int r, Node &n) const {
    if (l == r) {
      n = Node{0U, a[l]};
    } else {
      n = Node{0U, 0U};
    }
  }

  const std::vector<u32> &a;
};

struct Add {
  void update(int l, int r, Node &n) {
    n.tag += d;
    n.sum += static_cast<u32>(r - l + 1) * d;
  }

  u32 d;
};

struct Sum {
  void update(int l, int r, Node &n) { result += n.sum; }

  u32 result = 0;
};

struct SegmentTree : public SegmentTreeBase<Node, SegmentTree> {
  SegmentTree(const std::vector<u32> &a) : SegmentTreeBase(a.size()) {
    traverse_all(Build{a});
  }

  static void propagate(int l, int m, int r, Node &n, Node &ln, Node &rn) {
    if (n.tag) {
      ln.tag += n.tag;
      ln.sum += static_cast<u32>(m - l + 1) * n.tag;
      rn.tag += n.tag;
      rn.sum += static_cast<u32>(r - (m + 1) + 1) * n.tag;
      n.tag = 0;
    }
  }

  static void collect(int l, int m, int r, Node &n, Node &ln, Node &rn) {
    n.sum = ln.sum + rn.sum;
  }
};

TEST_CASE("segment_tree") {
  constexpr int n = 1000;

  std::mt19937 gen(0);

  std::vector<u32> a(n);
  for (int i = 0; i < n; ++i) {
    a[i] = gen();
  }
  SegmentTree tree(a);
  for (int _ = 0; _ < n; ++_) {
    int l = gen() % n;
    int r = gen() % n;
    if (l > r) {
      std::swap(l, r);
    }
    if (gen() & 1) {
      u32 d = gen();
      tree.traverse(Add{d}, l, r);
      for (int i = l; i <= r; ++i) {
        a[i] += d;
      }
    } else {
      Sum sum;
      tree.traverse(sum, l, r);
      u32 answer = 0;
      for (int i = l; i <= r; ++i) {
        answer += a[i];
      }
      REQUIRE(sum.result == answer);
    }
  }
}
