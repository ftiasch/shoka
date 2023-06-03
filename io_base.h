#pragma once

#include "is_specialization_of.h"

#include <tuple>
#include <vector>

#ifndef YES
#define YES "Yes"
#endif
#ifndef NO
#define NO "No"
#endif

template <typename IO> struct IOBaseT {
  template <typename T = int> T read(T &&v = T{}) {
    using DT = std::decay_t<T>;
    if constexpr (is_specialization_of_v<std::tuple, DT>) {
      read_t_(v, std::make_index_sequence<std::tuple_size_v<DT>>());
    } else if constexpr (is_specialization_of_v<std::vector, DT>) {
      for (auto &e : v) {
        read(e);
      }
    } else {
      static_cast<IO *>(this)->template read1(std::forward<T>(v));
    }
    return v;
  }

  template <typename T> IOBaseT &operator<<(const T &o) {
    if constexpr (std::is_same_v<bool, T>) {
      return static_cast<IO *>(this)->write1(o ? YES : NO), *this;
    } else if constexpr (is_specialization_of_v<std::vector, T>) {
      bool first = true;
      for (auto &&e : o) {
        if (first) {
          first = false;
        } else {
          static_cast<IO *>(this)->template write1(' ');
        }
        static_cast<IO *>(this)->template write1(e);
      }
      return *this;
    } else {
      return static_cast<IO *>(this)->template write1(o), *this;
    }
  }

  // helper

  template <typename T = int> std::vector<T> read_v(int n) {
    return read(std::vector<T>(n));
  }

  template <typename... Ts> std::tuple<Ts...> read_t() {
    return read(std::tuple<Ts...>{});
  }

private:
  template <typename Tuple, std::size_t... Index>
  void read_t_(Tuple &t, std::index_sequence<Index...>) {
    (..., (std::get<Index>(t) = read(std::tuple_element_t<Index, Tuple>{})));
  }
};
