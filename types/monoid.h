#pragma once

#include <concepts>

template <typename M>
concept IsMonoid = requires(M a, M b) {
  { M::plus(a, b) } -> std::same_as<M>;
};
