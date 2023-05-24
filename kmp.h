#pragma once

#include <string>
#include <vector>

std::vector<int> border_table(int n, const char *s) {
  std::vector<int> nxt(n, -1);
  for (int i = 1; i < n; ++i) {
    auto &j = nxt[i] = i - 1;
    while (~j) {
      j = nxt[j];
      if (s[j + 1] == s[i]) {
        j++;
        break;
      }
    }
  }
  return nxt;
}

std::vector<int> border_table(const std::string &s) {
  return border_table(s.size(), s.c_str());
}
