#include <array>

template <typename T, int... NS> struct NDArray;

template <typename T, int N0, int... NS> struct NDArray<T, N0, NS...> {
  using Nested = NDArray<T, NS...>;
  using Array = std::array<typename Nested::Array, N0>;

  static Array create() { return Array(); }
};

template <typename T, int N0> struct NDArray<T, N0> {
  using Array = std::array<T, N0>;

  static Array create() { return Array(); }
};
