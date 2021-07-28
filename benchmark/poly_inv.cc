#include "../mod.h"
#include "../ntt.h"

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench/picobench.hpp"

#include <vector>

static const int MOD = 998244353;

using ModT = montgomery::Montgomery32T<MOD>;
// using ModT = mod::ModT<MOD>;

using namespace ntt;

template <typename Inverse> static void poly_inv(picobench::state &s) {
  int n = 1 << s.iterations();
  std::vector<ModT> p(n), q(n);
  std::mt19937 gen(0);
  p[0] = ModT(gen() % (MOD - 1) + 1);
  for (int i = 1; i < n; ++i) {
    p[i] = ModT(gen() % MOD);
  }
  Inverse poly_inv(n);
  s.start_timer();
  poly_inv(n, p.data(), q.data());
  s.stop_timer();
}

PICOBENCH(poly_inv<InverseV0<NTT<ModT>>>);
