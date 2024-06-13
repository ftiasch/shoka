#pragma once

#include <concepts>

template <typename M>
concept IsTreeMonoid = requires(M a, M b) {
  { M::rake(a, b) } -> std::same_as<M>;
  { M::compress(a, b) } -> std::same_as<M>;
};
