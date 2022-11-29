#include <array>
#include <functional>
#include <vector>

template <typename T, typename Compare = std::less<T>>
struct MaxCartesianTree : public std::vector<std::array<int, 2>> {
  explicit MaxCartesianTree(const std::vector<T> &a,
                            Compare compare = Compare{})
      : std::vector<std::array<int, 2>>(a.size(), std::array<int, 2>{-1, -1}) {
    int n = a.size();
    std::vector<int> stack;
    stack.reserve(n);
    for (int i = 0; i < n; ++i) {
      int left = -1;
      while (!stack.empty() && compare(a[stack.back()], a[i])) {
        int j = stack.back();
        stack.pop_back();
        (*this)[j][1] = left;
        left = j;
      }
      (*this)[i][0] = left;
      stack.push_back(i);
    }
    root = -1;
    while (!stack.empty()) {
      int i = stack.back();
      stack.pop_back();
      (*this)[i][1] = root;
      root = i;
    }
  }

  int root;
};
