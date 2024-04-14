#include <limits>
#include <vector>

template <typename T, template <typename> class SolverT> struct MinPlusConv {
  struct Monge {
    using E = T;

    T operator()(int k, int i) const {
      constexpr T inf = std::numeric_limits<T>::max();
      auto j = k - i;
      if (0 <= j && j < w.size()) {
        return a[i] + w[k - i];
      }
      return j < 0 ? inf : inf - i;
    }

    const std::vector<T> &a, &w;
  };

  using Seq = std::vector<T>;

  // Assume that w[i] - w[i - 1] <= w[i + 1] - w[i]
  Seq operator()(const Seq &a, const Seq &w) {
    return solve(Monge{a, w}, a.size() + w.size() - 1, a.size());
  }

  SolverT<Monge> solve;
};
