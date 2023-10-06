#include "empty_class.h"

#include <functional>
#include <utility>
#include <vector>

template <typename Data_> struct LongestPathT {
  using Data = Data_;

  explicit LongestPathT(Data *data_) : data{data_} {}

  Data &operator[](int i) { return data[i]; }
  const Data &operator[](int i) const { return data[i]; }

  int len{0};
  Data *data;
};

template <typename Path, typename Impl> struct LongestPathDecompT {
  using Tree = std::vector<std::vector<int>>;
  using Data = typename Path::Data;

  explicit LongestPathDecompT(const Tree &tree_, int root_ = 0)
      : tree{tree_}, root{root_}, n(tree.size()), height(n), prefer(n, -1),
        buffer(n) {}

  void work() {
    std::function<void(int, int)> prepare = [&](int p, int u) -> void {
      std::pair<int, int> best{0, -1};
      for (auto &&v : tree[u]) {
        if (v != p) {
          prepare(u, v);
          best = std::max(best, {height[v], v});
        }
      }
      height[u] = 1 + best.first;
      prefer[u] = best.second;
    };
    prepare(-1, root);
    auto free_ptr = buffer.data() + n;
    std::function<Path(int, int, Data *)> dfs = [&](int p, int u,
                                                    Data *begin) -> Path {
      if (begin == nullptr) {
        free_ptr -= height[u];
        begin = free_ptr;
      }
      Path path{begin};
      if (~prefer[u]) {
        path = dfs(u, prefer[u], begin);
        for (auto &&v : tree[u]) {
          if (v != p && v != prefer[u]) {
            impl()->rake(path, dfs(u, v, nullptr), u, v);
          }
        }
      }
      impl()->append(path, u);
      return path;
    };
    dfs(-1, root, nullptr);
  }

  Impl *impl() { return static_cast<Impl *>(this); }

  const Tree &tree;
  int root, n;
  std::vector<int> height, prefer;
  std::vector<Data> buffer;
};

// Example: https://atcoder.jp/contests/arc086/submissions/46251455
