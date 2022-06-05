#include <algorithm>
#include <vector>

template <typename T> struct Discretization : public std::vector<T> {
  using std::vector<T>::begin;
  using std::vector<T>::end;

  Discretization(const std::vector<T> &a_) : std::vector<T>(a_) {
    std::sort(begin(), end());
    std::vector<T>::erase(std::unique(begin(), end()), end());
  }

  int index(T x) const { return std::lower_bound(begin(), end(), x) - begin(); }
};
