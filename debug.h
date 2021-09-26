#include <iostream>

#define DEBUG(expr)                                                            \
  do {                                                                         \
    std::cerr << "[L" << __LINE__ << "] " << expr << std::endl;                                            \
  } while (0)

#define KV(x) "|" << #x << "=" << x
