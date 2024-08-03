#include "types/compare.h"

#include <array>
#include <functional>
#include <ranges>
#include <vector>

template <typename T, typename C = std::less<T>>
  requires IsComparator<C, T>
class MaxCartesianTree {
  int n;
  std::vector<int> stack;

public:
  explicit MaxCartesianTree(std::ranges::random_access_range auto a,
                            C compare = {})
      : n(std::ranges::size(a)), stack(n), root{-1}, child(n) {
    int top = -1;
    for (int i = 0; i < n; i++) {
      while (~top && compare(a[stack[top]], a[i])) {
        top--;
      }
      int &r = ~top ? child[stack[top]][1] : root;
      child[i] = {r, -1};
      r = i;
      stack[++top] = i;
    }
  }

  int root; // virtually child[-1][1]
  std::vector<std::array<int, 2>> child;
};
