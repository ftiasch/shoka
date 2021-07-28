#include "../mod.h"

#include <random>

#include "gtest/gtest.h"

DECLARE_NON_CONST_MODT(ModT);

namespace {

static const int MOD = 998244353;

TEST(Montgomery, Test) {
  using ModT = montgomery::Montgomery32T<MOD>;

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

TEST(ModT, Test) {
  using ModT = mod::ModT<MOD>;

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

TEST(ModT, NonConstModTest) {
  ModT::MOD = MOD;

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

} // namespace
