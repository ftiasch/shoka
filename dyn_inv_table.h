#pragma once

#include "singleton.h"

#include <vector>

template <typename Mod> struct DynInvTable {
  explicit DynInvTable() : b(2) { b[1] = Mod{1}; }

  Mod operator[](int i) {
    for (int j = b.size(); j <= i; j++) {
      b.push_back(-Mod{Mod::mod() / j} * b[Mod::mod() % j]);
    }
    return b[i];
  }

  std::vector<Mod> b;
};

template <typename Mod> static Mod inv(int a) {
  return singleton<DynInvTable<Mod>>()[a];
}
