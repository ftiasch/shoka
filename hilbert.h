#include <cstdint>

static inline uint64_t hilbert_rank(int x, int y, int k = 31, int rotate = 0) {
  if (k--) {
    static const int INDEX[2][2] = {{0, 3}, {1, 2}};
    static const int DELTA[4] = {3, 0, 0, 1};
    int index = INDEX[x >> k][y >> k] + rotate & 3;
    uint64_t result = hilbert_rank(x & ~(1 << k), y & ~(1 << k), k,
                                   rotate + DELTA[index] & 3);
    return (static_cast<uint64_t>(index) << (k + k)) +
           ((index == 1 || index == 2)
                ? result
                : ((static_cast<uint64_t>(1) << (k + k)) - 1 - result));
  }
  return 0;
}
