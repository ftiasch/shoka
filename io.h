#pragma once

#include "io_base.h"

#include <iomanip>
#include <iostream>

struct IO : public IOBaseT<IO> {
  friend class IOBaseT<IO>;

  explicit IO(bool sync = false) {
    if (!sync) {
      std::ios::sync_with_stdio(false);
      std::cin.tie(nullptr);
    }
    std::cout << std::fixed << std::setprecision(12);
  }

private:
  template <typename T> void read1(T &&v) { std::cin >> v; }
  template <typename T> void write1(T &&v) { std::cout << v; }
};
