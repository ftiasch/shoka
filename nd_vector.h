#include <algorithm>
#include <cstddef>
#include <functional>
#include <vector>

template <typename T, std::size_t N = 2>
struct NDVector : public std::vector<NDVector<T, N - 1>> {
  using Nested = NDVector<T, N - 1>;

  template <typename... Args>
  NDVector(std::size_t n, Args &&...args)
      : std::vector<Nested>(n, Nested(std::forward<Args>(args)...)) {}
};

template <typename T> struct NDVector<T, 1> : public std::vector<T> {
  template <typename... Args>
  NDVector(Args &&...args) : std::vector<T>(std::forward<Args>(args)...) {}
};
