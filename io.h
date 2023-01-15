#pragma once

#include "io_base.h"

#include <iostream>

struct IO : public IOBaseT<IO> {
  explicit IO(bool sync = false) {
    if (sync) {
      std::ios::sync_with_stdio(false);
      std::cin.tie(nullptr);
      std::cout.tie(nullptr);
    }
  }

  template <typename T> void read1(T &&v) { std::cin >> v; }

  template <typename T> std::ostream &operator<<(const T &o) {
    return std::cout << o;
  }
};
