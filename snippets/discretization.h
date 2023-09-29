#include <algorithm>
#include <vector>

template <typename T, typename Compare = std::less<T>>
struct Discretization : public std::vector<T> {
  explicit Discretization() = default;

  explicit Discretization(const std::vector<T> &a_) : std::vector<T>(a_) {
    normalize();
  }

  void normalize() {
    std::ranges::sort(*this, Compare{});
    auto [first, last] = std::ranges::unique(*this);
    this->erase(first, last);
  }

  int index(T x) const {
    return std::ranges::lower_bound(*this, x, Compare{}) - this->begin();
  }
};
