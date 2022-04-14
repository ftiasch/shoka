#include <cstdint>
#include <utility>
#include <vector>

template <int M>
struct SuffixXorBasis : public std::vector<std::pair<uint64_t, int>> {
  SuffixXorBasis() : std::vector<std::pair<uint64_t, int>>(M, {0, -1}) {}

  void add(uint64_t v, int vi) {
    for (int k = M; k--;) {
      if (v >> k & 1) {
        auto &[b, bi] = (*this)[k];
        if (bi < vi) {
          std::swap(v, b);
          std::swap(vi, bi);
        }
        v ^= b;
      }
    }
  }
};
