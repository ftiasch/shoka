#pragma once

#include <concepts>

template <typename A, typename E>
concept Is2dMatrixElem = requires(A m, int i, int j) {
  { m(i, j) } -> std::same_as<E>;
} && std::totally_ordered<E>;

template <typename A>
concept Is2dMatrix = requires {
  typename A::E;
  requires Is2dMatrixElem<A, typename A::E>;
};
