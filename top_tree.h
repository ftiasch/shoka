#include "types/tree_monoid.h"

#include <algorithm>
#include <vector>

using Tree = std::vector<std::vector<int>>;

// Assume that there's an imaginary edge (-1)->0
template <IsTreeMonoid M> struct TopTreeBase {
  // assume vertices is numbered by 0 ... n - 1
  explicit TopTreeBase(const Tree &tree_)
      : tree{tree_}, n(tree.size()), parent(n, -1), size(n), preferred_child(n),
        cluster(n) {
    cluster.reserve(n + n);
  }

  M root() const { return root_->sum; }

  void build(const std::vector<M> &edge, int r = 0) {
    prepare(r);
    for (int i = 0; i < n; i++) {
      cluster[i].sum = edge[i];
    }
    root_ = build_tree(r);
  }

  void update1(int u, const M &m) {
    auto p = cluster.data() + u;
    p->sum = m;
    p = p->parent;
    while (p != nullptr) {
      update(p);
      p = p->parent;
    }
  }

private:
  enum class Type {
    BASE,
    COMPRESS,
    RAKE,
  };

  struct TreeNode {
    explicit TreeNode() : type{Type::BASE} {}

    explicit TreeNode(Type type_, TreeNode *lc, TreeNode *rc)
        : type{type_}, child{lc, rc} {
      lc->parent = rc->parent = this;
    }

    M sum;
    Type type;
    TreeNode *parent{nullptr};
    std::array<TreeNode *, 2> child{nullptr, nullptr};
  };

  void prepare(int u) {
    size[u] = 1;
    std::pair<int, int> best{0, u};
    for (auto &&v : tree[u]) {
      if (v != parent[u]) {
        parent[v] = u;
        prepare(v);
        best = std::max(best, {size[v], v});
        size[u] += size[v];
      }
    }
    preferred_child[u] = best.second;
  }

  using Chain = std::vector<std::pair<int, TreeNode *>>;

  TreeNode *build_tree(int u) {
    Chain compress_chain{{1, cluster.data() + u}};
    while (preferred_child[u] != u) {
      Chain rake_chain{{1, cluster.data() + preferred_child[u]}};
      rake_chain.reserve(tree[u].size());
      for (auto &&v : tree[u]) {
        if (v != parent[u] && v != preferred_child[u]) {
          rake_chain.emplace_back(size[v], build_tree(v));
        }
      }
      compress_chain.emplace_back(size[u] - size[preferred_child[u]],
                                  reduce<Type::RAKE>(rake_chain));
      u = preferred_child[u];
    }
    return reduce<Type::COMPRESS>(compress_chain);
  }

  template <Type type> TreeNode *reduce_(const Chain &chain, int l, int r) {
    if (l + 1 == r) {
      return chain[l].second;
    }
    auto begin = l ? chain[l - 1].first : 0;
    auto mid = (begin + chain[r - 1].first) >> 1;
    auto m = std::upper_bound(chain.begin() + l + 1, chain.begin() + r,
                              std::pair<int, TreeNode *>{mid, nullptr}) -
             chain.begin();
    cluster.emplace_back();
    return update(new (&cluster.back()) TreeNode{
        type, reduce_<type>(chain, l, m), reduce_<type>(chain, m, r)});
  }

  template <Type type> TreeNode *reduce(Chain &chain) {
    for (int i = 1; i < chain.size(); i++) {
      chain[i].first += chain[i - 1].first;
    }
    return reduce_<type>(chain, 0, chain.size());
  }

  TreeNode *update(TreeNode *u) {
    auto [lc, rc] = u->child;
    switch (u->type) {
    case Type::COMPRESS: {
      u->sum = M::compress(lc->sum, rc->sum);
      break;
    }
    case Type::RAKE: {
      u->sum = M::rake(lc->sum, rc->sum);
      break;
    }
    }
    return u;
  }

  const Tree &tree;
  int n;
  std::vector<int> parent, size, preferred_child;
  std::vector<TreeNode> cluster;
  TreeNode *root_;
};
