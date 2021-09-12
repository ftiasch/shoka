#include <cstdint>

#ifdef LOCAL
#define DEBUG(v)                                                               \
  do {                                                                         \
    std::cerr << "[L" << __LINE__ << "]" << #v << "=" << v << std::endl;       \
  } while (0)
#else
#define DEBUG(v)
#endif
