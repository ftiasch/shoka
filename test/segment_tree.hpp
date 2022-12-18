#include "segment_tree.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators_all.hpp>

namespace segment_tree {

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
  explicit SegmentTree(const std::vector<u32> &a) : SegmentTreeBase(a.size()) {
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

} // namespace segment_tree

TEST_CASE("segment_tree") {
  constexpr int n = 1000;

  using namespace Catch::Generators;
  auto a = GENERATE(take(1, chunk(n, random(0U, ~0U))));
  auto L = GENERATE(take(1, chunk(n, random(0, n - 1))));
  auto R = GENERATE(take(1, chunk(n, random(0, n - 1))));
  auto D = GENERATE(take(1, chunk(n, random(0U, ~0U))));

  using namespace segment_tree;
  SegmentTree tree(a);
  for (int q = 0; q < n; ++q) {
    auto [l, r] = std::tie(L[q], R[q]);
    if (l > r) {
      std::swap(l, r);
    }
    if (D[q] & 1) {
      tree.traverse(Add{D[q]}, l, r);
      for (int i = l; i <= r; ++i) {
        a[i] += D[q];
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
