template <int... INTS> struct HashT {
  static constexpr int M = sizeof...(INTS);
  static constexpr std::array<int, M> MODS = {INTS...};

  explicit HashT(int a) {
    for (int i = 0; i < M; ++i) {
      h[i] = a % MODS[i];
    }
  }

  int h[M];
};

template <int... INTS>
bool operator<(const HashT<INTS...> &a, const HashT<INTS...> &b) {
  for (int i = 0; i < HashT<INTS...>::M; ++i) {
    if (a.h[i] != b.h[i]) {
      return a.h[i] < b.h[i];
    }
  }
  return false;
}
