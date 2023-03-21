#pragma once

#include "is_specialization_of.h"

#include <tuple>
#include <vector>

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
      reinterpret_cast<IO *>(this)->template read1(std::forward<T>(v));
    }
    return v;
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
