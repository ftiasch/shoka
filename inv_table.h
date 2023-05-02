#pragma once

#include <vector>

template <typename Mod> struct InvTable : std::vector<Mod> {
  explicit InvTable(int n) : std::vector<Mod>(n) {
    (*this)[1] = Mod{1};
    for (int i = 2; i < n; ++i) {
      (*this)[i] = -Mod{Mod::mod() / i} * (*this)[Mod::mod() % i];
    }
  }
};
