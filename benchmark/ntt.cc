#include "../ntt.h"
#include "../mod.h"

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench/picobench.hpp"

#include <vector>

static const int MOD = 998244353;

using ModT = montgomery::Montgomery32T<MOD>;

using namespace ntt;

static void poly_inv(picobench::state &s) {
  int n = 1 << s.iterations();
  std::vector<ModT> p(n), q(n);
  std::mt19937 gen(0);
  for (int i = 0; i < n; ++i) {
    p[i] = ModT(gen() % MOD);
  }
  Inverse<NTT<ModT>> poly_inv(n);
  s.start_timer();
  poly_inv(n, p.data(), q.data());
  s.stop_timer();
}

PICOBENCH(poly_inv);
