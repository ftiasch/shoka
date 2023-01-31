#pragma once

#include "is_specialization_of.h"

#include <tuple>
#include <vector>

template <typename IO> struct IOBaseT {
  template <typename T_ = int>
  std::decay_t<T_> read(T_ &&v = std::decay_t<T_>{}) {
    using T = std::decay_t<T_>;
    if constexpr (is_specialization_of_v<std::tuple, T>) {
      read_t_(v, std::make_index_sequence<std::tuple_size_v<T>>());
    } else if constexpr (is_specialization_of_v<std::vector, T>) {
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
