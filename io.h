#pragma once

#include "io_base.h"

#include <iostream>

#ifndef YES
#define YES "Yes"
#endif
#ifndef NO
#define NO "No"
#endif

struct IO : public IOBaseT<IO> {
  friend class IOBaseT<IO>;

  explicit IO(bool sync = false) {
    if (!sync) {
      std::ios::sync_with_stdio(false);
      std::cin.tie(nullptr);
      std::cout.tie(nullptr);
    }
  }

  template <typename T> std::ostream &operator<<(const T &o) {
    if constexpr (std::is_same_v<bool, T>) {
      return std::cout << (o ? YES : NO);
    } else if constexpr (is_specialization_of_v<std::vector, T>) {
      bool first = true;
      for (auto &&e : o) {
        if (first) {
          first = false;
        } else {
          std::cout << " ";
        }
        std::cout << e;
      }
      return std::cout;
    } else {
      return std::cout << o;
    }
  }

private:
  template <typename T> void read1(T &&v) { std::cin >> v; }
};
