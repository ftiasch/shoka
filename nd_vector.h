#include <algorithm>
#include <functional>
#include <vector>

template <typename T, int N> struct NDVector {
  using Nested = NDVector<T, N - 1>;
  using Vector = std::vector<typename Nested::Vector>;

  template <typename... Args> static Vector create(int n, Args &&...args) {
    return Vector(n, Nested::create(std::forward<Args>(args)...));
  }

  static void fill(Vector &v, T v0) {
    std::for_each(v.begin(), v.end(),
                  std::bind(Nested::fill, std::placeholders::_1, v0));
  }
};

template <typename T> struct NDVector<T, 1> {
  using Vector = std::vector<T>;

  template <typename... Args> static Vector create(Args &&...args) {
    return Vector(std::forward<Args>(args)...);
  }

  static void fill(Vector &v, T v0) { std::fill(v.begin(), v.end(), v0); }
};
