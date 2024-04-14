#include "smawk.h"

#include <limits>
#include <vector>

template <typename E_> struct MinPlusConv {
  struct Monge {
    using E = E_;

    E operator()(int k, int i) const {
      constexpr E inf = std::numeric_limits<E>::max();
      auto j = k - i;
      if (0 <= j && j < w.size()) {
        return a[i] + w[k - i];
      }
      return j < 0 ? inf : inf - i;
    }

    const std::vector<E> &a, &w;
  };

  using Seq = std::vector<E_>;

  // Assume that w[i] - w[i - 1] <= w[i + 1] - w[i]
  Seq operator()(const Seq &a, const Seq &w) {
    int n = a.size() + w.size() - 1;
    auto row_min = smawk(Monge{a, w}, n, a.size());
    Seq c(n);
    for (int i = 0; i < n; i++) {
      c[i] = row_min[i].first;
    }
    return c;
  }

  SMAWK<Monge> smawk;
};
