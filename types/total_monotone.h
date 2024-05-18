#pragma once

#include <concepts>

template <typename A, typename E>
concept IsTMElem = requires(A m, int i, int j) {
  { m(i, j) } -> std::same_as<E>;
} && std::totally_ordered<E>;

template <typename A>
concept IsTM = requires {
  typename A::E;
  requires IsTMElem<A, typename A::E>;
};
