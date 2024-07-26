#pragma once

#include <functional>
#include <ranges>

template <std::ranges::random_access_range Range>
void group(const Range &v, const std::function<void(int, int)> &fn) {
  int n = v.size();
  for (int i = 0; i < n;) {
    auto i0 = i;
    while (i < n && v[i0] == v[i]) {
      i++;
    }
    fn(i0, i);
  }
}
