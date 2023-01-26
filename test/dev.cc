#include "poly.h"

#include "singleton.h"

#include <memory>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "debug.h"

template <typename Mod> struct PolyGenT {
  using Vector = std::vector<Mod>;
  using Poly = PolyT<Mod>;

  struct Op;

  using OpPtr = std::shared_ptr<Op>;

  struct Op {
    explicit Op(bool acyclic_) : acyclic{acyclic_} {}

    virtual ~Op() = default;

    virtual Mod at(int i) { return compute(i); }

    virtual void delegate(OpPtr) {}

    const bool acyclic;

  protected:
    virtual Mod compute(int) = 0;
  };

  struct CachedOp : public Op {
    using Op::compute;

    using Op::Op;

    Mod at(int i) override {
      while (cache.size() <= i) {
        cache.push_back(compute(cache.size()));
      }
      return cache[i];
    }

  private:
    std::vector<Mod> cache;
  };

  struct Dummy : public CachedOp {
    explicit Dummy() : CachedOp{false} {}

    void delegate(OpPtr op) override { weak = op; }

  private:
    Mod compute(int i) override {
      if (auto d = weak.lock()) {
        return d->at(i);
      } else {
        throw std::runtime_error("The delegator is invalid.");
      }
    }

    std::weak_ptr<Op> weak;
  };

  struct Value : public Op {
    explicit Value(const Vector &c_) : Op{true}, c{c_} {}

  private:
    Mod compute(int i) override {
      return i < static_cast<int>(c.size()) ? c[i] : Mod{0};
    }

    Vector c;
  };

  struct Shift : public Op {
    explicit Shift(OpPtr p_, int s_)
        : Op{p_->acyclic}, p{std::move(p_)}, s{s_} {}

  private:
    Mod compute(int i) override { return i < s ? Mod{0} : p->at(i - s); }

    OpPtr p;
    int s;
  };

  struct Add : public Op {
    explicit Add(OpPtr p_, OpPtr q_)
        : Op{p_->acyclic && q_->acyclic}, p{std::move(p_)}, q{std::move(q_)} {}

  private:
    Mod compute(int i) override { return p->at(i) + q->at(i); }

    OpPtr p, q;
  };

  struct Sub : public Op {
    explicit Sub(OpPtr p_, OpPtr q_)
        : Op{p_->acyclic && q_->acyclic}, p{std::move(p_)}, q{std::move(q_)} {}

  private:
    Mod compute(int i) override { return p->at(i) - q->at(i); }

    OpPtr p, q;
  };

  struct ModInv : public CachedOp {
    using Op::at;

    explicit ModInv() : CachedOp{true} {}

    Mod compute(int i) override {
      return i < 2 ? Mod{i}
                   : Mod{Mod::mod() - Mod::mod() / i} * at(Mod::mod() % i);
    }
  };

  struct Integral : public Op {
    explicit Integral(OpPtr p_, Mod c_)
        : Op{p_->acyclic}, p{std::move(p_)}, c{c_} {}

  private:
    Mod compute(int i) override {
      return i ? p->at(i - 1) * singleton<ModInv>().at(i) : c;
    }

    OpPtr p;
    Mod c;
  };

  struct Mul : public Op {
    explicit Mul(OpPtr p_, OpPtr q_)
        : Op{true}, p{std::move(p_)}, q{std::move(q_)} {}

  private:
    Mod compute(int i) override {
      if (known.size() <= i) {
        auto n = Poly::min_power_of_two(i + 1);
        Poly::reserve(n << 1);
        Poly pp(n), qq(n);
        for (int i = 0; i < n; ++i) {
          pp[i] = p->at(i);
          qq[i] = q->at(i);
        }
        known = pp * qq;
        known.resize(n);
      }
      return known[i];
    }

    OpPtr p, q;

    Poly known;
  };

  struct OnlineMul : public Op {
    explicit OnlineMul(OpPtr p_, OpPtr q_)
        : Op{false}, p{std::move(p_)}, q{std::move(q_)}, known{0} {}

  private:
    Mod compute(int i) override {
      if (known <= i) {
        auto n = Poly::min_power_of_two(i + 1);
        result.resize(n);
        buffer[0].resize(n);
        buffer[1].resize(n);
        while (known <= i) {
          recur(0, n, known++);
        }
      }
      return result[i];
    }

  protected:
    virtual void recur(int, int, int) = 0;

    OpPtr p, q;
    int known;
    std::vector<Mod> result;
    std::array<std::vector<Mod>, 2> buffer;
  };

  struct SemiMul : public OnlineMul {
    using OnlineMul::OnlineMul;

  private:
    using OnlineMul::result, OnlineMul::buffer, OnlineMul::p, OnlineMul::q;

    void recur(int l, int r, int k) override {
      if (l + 1 == r) {
        result[l] += q->at(0) == Mod{0} ? Mod{0} : p->at(l) * q->at(0);
      } else {
        auto m = (l + r) >> 1;
        if (k < m) {
          recur(l, m, k);
        } else {
          if (k == m) {
            auto n = m - l;
            Poly::reserve(n << 1);
            auto b0 = buffer[0].data();
            auto b1 = buffer[1].data();
            for (int i = 0; i < n; ++i) {
              b0[i] = p->at(l + i);
            }
            std::fill(b0 + n, b0 + (n << 1), Mod{0});
            for (int i = 0; i < n << 1; ++i) {
              b1[i] = q->at(i);
            }
            Poly::dif(n << 1, b0);
            Poly::dif(n << 1, b1);
            Poly::dot_product_and_dit(n << 1, b0, b0, b1);
            for (int i = m; i < r; ++i) {
              result[i] += b0[i - l];
            }
          }
          recur(m, r, k);
        }
      }
    }
  };

  struct FullMul : public OnlineMul {
    using OnlineMul::OnlineMul;

  private:
    using OnlineMul::buffer, OnlineMul::p, OnlineMul::q;

    void recur(int l, int r, int k) override {}
  };

  struct Wrapper {
    explicit Wrapper(OpPtr op_) : op{std::move(op_)} {}

    auto operator[](int i) { return op->at(i); }

    auto operator+(const Wrapper &o) {
      return Wrapper{std::make_shared<Add>(op, o.op)};
    }

    auto operator-(const Wrapper &o) {
      return Wrapper{std::make_shared<Sub>(op, o.op)};
    }

    auto operator*(const Wrapper &o) { return Wrapper{smart_mul(op, o.op)}; }

    auto integrate(Mod c = Mod{0}) {
      return Wrapper{std::make_shared<Integral>(op, c)};
    }

    auto shift(int s) { return Wrapper{std::make_shared<Shift>(op, s)}; }

    // NOTE: Not using const reference to prohibit r-values.
    void delegate(Wrapper &o) { op->delegate(o.op); }

  private:
    static OpPtr smart_mul(OpPtr p, OpPtr q) {
      if (p->acyclic) {
        p.swap(q);
      }
      if (p->acyclic) {
        return std::make_shared<Mul>(p, q);
      }
      if (q->acyclic) {
        return std::make_shared<SemiMul>(p, q);
      }
      return std::make_shared<FullMul>(p, q);
    }

    OpPtr op;
  };

  static auto value(const std::vector<Mod> &values) {
    return Wrapper{std::make_shared<Value>(values)};
  }

  static auto dummy() { return Wrapper{std::make_shared<Dummy>()}; }
};

#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("poly_gen") {
  using Mod = ModT<998'244'353>;
  using PolyGen = PolyGenT<Mod>;

  auto take = [&](PolyGen::Wrapper w, int n) {
    std::vector<Mod> v(n);
    for (int i = 0; i < n; ++i) {
      v[i] = w[i];
    }
    return v;
  };

  SECTION("const") {
    auto p = PolyGen::value({Mod{1}, Mod{2}});
    REQUIRE(take(p, 5) ==
            std::vector<Mod>{Mod{1}, Mod{2}, Mod{0}, Mod{0}, Mod{0}});
  }

  SECTION("add") {
    auto p = PolyGen::value({Mod{1}, Mod{0}, Mod{3}});
    auto q = PolyGen::value({Mod{0}, Mod{2}});
    auto r = p + q;
    REQUIRE(take(r, 5) ==
            std::vector<Mod>{Mod{1}, Mod{2}, Mod{3}, Mod{0}, Mod{0}});
  }

  SECTION("add_self") {
    auto p = PolyGen::value({Mod{1}});
    auto r = p + p;
    REQUIRE(take(r, 5) ==
            std::vector<Mod>{Mod{2}, Mod{0}, Mod{0}, Mod{0}, Mod{0}});
  }

  SECTION("sub") {
    auto p = PolyGen::value({Mod{2}, Mod{3}});
    auto q = PolyGen::value({Mod{1}});
    auto r = p - q;
    REQUIRE(take(r, 5) ==
            std::vector<Mod>{Mod{1}, Mod{3}, Mod{0}, Mod{0}, Mod{0}});
  }

  SECTION("integral") {
    auto p = PolyGen::value({Mod{2}, Mod{3}});
    auto r = p.integrate(Mod{1});
    REQUIRE(r[0] == Mod{1});
    REQUIRE(r[1] == Mod{2});
    REQUIRE(r[2] * Mod{2} == Mod{3});
  }

  SECTION("shift") {
    auto p = PolyGen::value({Mod{2}, Mod{3}});
    auto r = p.shift(1);
    REQUIRE(take(r, 5) ==
            std::vector<Mod>{Mod{0}, Mod{2}, Mod{3}, Mod{0}, Mod{0}});
  }

  SECTION("recur_geo_sum") {
    // f(z) = f(z) * z^2 + 1
    auto f = PolyGen::dummy();
    auto rhs = f.shift(2) + PolyGen::value({Mod{1}});
    f.delegate(rhs);
    REQUIRE(f[1000000] == Mod{1});
    REQUIRE(f[1000001] == Mod{0});
  }

  SECTION("mul") {
    // (1 + z)^4 = 1 + 4 z + 6 z^2 + 4 z^3 * z^4
    auto f = PolyGen::value({Mod{1}, Mod{1}});
    auto f2 = f * f;
    auto f4 = f2 * f2;
    REQUIRE(take(f4, 10) == std::vector<Mod>{Mod{1}, Mod{4}, Mod{6}, Mod{4},
                                             Mod{1}, Mod{0}, Mod{0}, Mod{0},
                                             Mod{0}, Mod{0}});
  }

  SECTION("recur_fib") {
    // f(z) = f(z) * (z + z^2) + 1
    auto f = PolyGen::dummy();
    auto rhs =
        f * PolyGen::value({Mod{0}, Mod{1}, Mod{1}}) + PolyGen::value({Mod{1}});
    f.delegate(rhs);
    REQUIRE(take(f, 6) ==
            std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}, Mod{8}});
  }

  SECTION("recur_fib_large") {
    // f(z) = f(z) * (z + z^2) + 1
    auto f = PolyGen::dummy();
    auto rhs =
        f * PolyGen::value({Mod{0}, Mod{1}, Mod{1}}) + PolyGen::value({Mod{1}});
    f.delegate(rhs);
    REQUIRE(f[100000] == Mod{56136314});
  }

  SECTION("recur_fib_2") {
    // f(z) = f(z) * (z + z^2) + 1
    auto f = PolyGen::dummy();
    auto rhs =
        PolyGen::value({Mod{0}, Mod{1}, Mod{1}}) * f + PolyGen::value({Mod{1}});
    f.delegate(rhs);
    REQUIRE(take(f, 6) ==
            std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}, Mod{8}});
  }

  SECTION("recur_fib_3") {
    // f(z) = (f(z) * (1 + z)) * z + 1
    auto f = PolyGen::dummy();
    auto rhs = (f * PolyGen::value({Mod{1}, Mod{1}})).shift(1) +
               PolyGen::value({Mod{1}});
    f.delegate(rhs);
    REQUIRE(take(f, 6) ==
            std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}, Mod{8}});
  }

  SECTION("recur_fib_4") {
    // f(z) = (f(z) * (1 + z)) * z + 1
    auto f = PolyGen::dummy();
    auto rhs = f.shift(1) * PolyGen::value({Mod{1}, Mod{1}}) +
               PolyGen::value({Mod{1}});
    f.delegate(rhs);
    REQUIRE(take(f, 6) ==
            std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}, Mod{8}});
  }

  SECTION("recur_fib_fg") {
    // f(z) = g(z) * z + 1
    // g(z) = f(z) * (1 + z)
    auto f = PolyGen::dummy();
    auto g = PolyGen::dummy();
    auto grhs = f * PolyGen::value({Mod{1}, Mod{1}});
    g.delegate(grhs);
    auto frhs = g.shift(1) + PolyGen::value({Mod{1}});
    f.delegate(frhs);
    REQUIRE(take(f, 6) ==
            std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}, Mod{8}});
  }

  /*
  SECTION("recur_catalan") {
    // f(z) = g(z) * z + 1
    // g(z) = f(z) * (1 + z)
    auto f = PolyGen::dummy();
    auto rhs = (f * f).shift(1) + PolyGen::value({Mod{1}});
    f.delegate(rhs);
    REQUIRE(take(f, 6) ==
            std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{5}, Mod{14},
  Mod{42}});
  }
  */
}
