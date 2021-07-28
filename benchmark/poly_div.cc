#include "../mod.h"
#include "../ntt.h"

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench/picobench.hpp"

#include <vector>

static const int MOD = 998244353;

using ModT = montgomery::Montgomery32T<MOD>;
// using ModT = mod::ModT<MOD>;

using namespace ntt;

template <typename Divide> static void poly_div(picobench::state &s) {
  int n = 1 << s.iterations();
  std::vector<ModT> p(n), q(n), r(n);
  std::mt19937 gen(0);
  for (int i = 0; i < n; ++i) {
    p[i] = ModT(gen() % MOD);
    q[i] = ModT(gen() % MOD);
  }
  q[0] = ModT(gen() % (MOD - 1) + 1);
  Divide poly_div(n);
  s.start_timer();
  poly_div(n, p.data(), q.data(), r.data());
  s.stop_timer();
}

PICOBENCH(poly_div<DivideV0<NTT<ModT>>>).label("v0").baseline();
PICOBENCH(poly_div<DivideV1<NTT<ModT>>>).label("v1");
