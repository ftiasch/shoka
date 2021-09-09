#include "../montgomery.h"
#include "../mod.h"
#include "../non_const_montgomery.h"

#include <random>

#include "gtest/gtest.h"

DECLARE_NON_CONST_MODT(NonConstMod);
DECLARE_NON_CONST_MONTGOMERY(NonConstMontgomery);

namespace {

static const int MOD = 998244353;

template <typename ModT> void perform_test() {
  std::mt19937_64 gen(0);
  uint32_t a = gen() % MOD;
  uint32_t b = gen() % MOD;
  ModT mz;
  ModT ma(a);
  ASSERT_EQ(ma.get(), a);
  ModT mb(b);
  ASSERT_EQ(mb.get(), b);
  ASSERT_EQ((ma + mb).get(), (a + b) % MOD);
  ModT mc = ma * mb;
  ASSERT_EQ(mc.get(), (uint64_t)a * b % MOD);
}

TEST(Montgomery, Test) { perform_test<montgomery::Montgomery32T<MOD>>(); }

TEST(Montgomery, NonConstTest) {
  NonConstMontgomery::set_mod(MOD);
  perform_test<NonConstMontgomery>();
}

TEST(ModT, Test) { perform_test<mod::ModT<MOD>>(); }

TEST(ModT, NonConstModTest) {
  NonConstMod::MOD = MOD;
  perform_test<NonConstMod>();
}

} // namespace
