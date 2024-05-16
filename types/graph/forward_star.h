#pragma once

#include <span>
#include <vector>

template <typename E = int> struct ForwardStarT {
  explicit ForwardStarT(int n_) : n{n_}, head(n + 1) {}
  int size() const { return n; }
  std::span<const E> operator[](int u) const {
    return std::span{edges}.subspan(head[u], head[u + 1] - head[u]);
  }

  template <typename... Args> void add(int u, Args... args) {
    head[u]++;
    from_.push_back(u);
    edges.emplace_back(std::forward<Args>(args)...);
  }

  void build() {
    int sum = 0;
    for (int i = 0; i <= n; i++) {
      head[i] = (sum += head[i]);
    }
    std::vector<E> new_edges(edges.size());
    for (int i = edges.size(); i--;) {
      new_edges[--head[from_[i]]] = edges[i];
    }
    edges.swap(new_edges);
  }

  int n;
  std::vector<int> head, from_;
  std::vector<E> edges;
};
