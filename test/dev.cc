#include "poly.h"

#include "singleton.h"

#include <memory>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

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

    auto slice(int l, int r) {
      Vector v(r - l);
      for (int i = l; i < r; ++i) {
        v[i - l] = at(i);
      }
      return v;
    }

  protected:
    virtual Mod compute(int) = 0;
  };

  struct CachedOp : public Op {
    using Op::compute;

    explicit CachedOp(bool acyclic) : Op{acyclic} {}

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
    explicit Dummy() : CachedOp{true} {}

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

  struct Const : public Op {
    explicit Const(const Vector &c_) : Op{true}, c{c_} {}

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
        : Op{p_->acyclic || q_->acyclic}, p{std::move(p_)}, q{std::move(q_)} {}

  private:
    Mod compute(int i) override { return p->at(i) + q->at(i); }

    OpPtr p, q;
  };

  struct Sub : public Op {
    explicit Sub(OpPtr p_, OpPtr q_)
        : Op{p_->acyclic || q_->acyclic}, p{std::move(p_)}, q{std::move(q_)} {}

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

  struct Int_ : public Op {
    explicit Int_(OpPtr p_, Mod c_)
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
        known = Poly{p->slice(0, n)} * Poly{q->slice(0, n)};
        known.resize(n);
      }
      return known[i];
    }

    OpPtr p, q;

    Poly known;
  };

  struct SemiMul : public Op {
    // q is acyclic
    explicit SemiMul(OpPtr p_, OpPtr q_)
        : Op{false}, p{std::move(p_)}, q{std::move(q_)} {}

  private:
    Mod compute(int i) override { return Mod{0}; }

    OpPtr p, q;
  };

  struct FullMul : public CachedOp {
    explicit FullMul(OpPtr p_, OpPtr q_)
        : CachedOp{false}, p{std::move(p_)}, q{std::move(q_)} {}

  private:
    Mod compute(int i) override { return Mod{0}; }

    OpPtr p, q;
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

    auto int_(Mod c = Mod{0}) { return Wrapper{std::make_shared<Int_>(op, c)}; }

    auto shift(int s) { return Wrapper{std::make_shared<Shift>(op, s)}; }

    // NOTE: Not using const reference to prohibit r-values.
    void delegate(Wrapper &o) { op->delegate(o.op); }

    auto slice(int l, int r) { return op->slice(l, r); }

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

  static auto const_(const Vector &c) {
    return Wrapper{std::make_shared<Const>(c)};
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

  SECTION("const") {
    auto p = PolyGen::const_({Mod{1}});
    REQUIRE(p.slice(0, 5) ==
            std::vector<Mod>{Mod{1}, Mod{0}, Mod{0}, Mod{0}, Mod{0}});
  }

  SECTION("add") {
    auto p = PolyGen::const_({Mod{1}, Mod{0}, Mod{3}});
    auto q = PolyGen::const_({Mod{0}, Mod{2}});
    auto r = p + q;
    REQUIRE(r.slice(0, 5) ==
            std::vector<Mod>{Mod{1}, Mod{2}, Mod{3}, Mod{0}, Mod{0}});
  }

  SECTION("add_self") {
    auto p = PolyGen::const_({Mod{1}});
    auto r = p + p;
    REQUIRE(r.slice(0, 5) ==
            std::vector<Mod>{Mod{2}, Mod{0}, Mod{0}, Mod{0}, Mod{0}});
  }

  SECTION("sub") {
    auto p = PolyGen::const_({Mod{2}, Mod{3}});
    auto q = PolyGen::const_({Mod{1}});
    auto r = p - q;
    REQUIRE(r.slice(0, 5) ==
            std::vector<Mod>{Mod{1}, Mod{3}, Mod{0}, Mod{0}, Mod{0}});
  }

  SECTION("integral") {
    auto p = PolyGen::const_({Mod{2}, Mod{3}});
    auto r = p.int_(Mod{1});
    REQUIRE(r[0] == Mod{1});
    REQUIRE(r[1] == Mod{2});
    REQUIRE(r[2] * Mod{2} == Mod{3});
  }

  SECTION("shift") {
    auto p = PolyGen::const_({Mod{2}, Mod{3}});
    auto r = p.shift(1);
    REQUIRE(r.slice(0, 5) ==
            std::vector<Mod>{Mod{0}, Mod{2}, Mod{3}, Mod{0}, Mod{0}});
  }

  SECTION("recur_geo_sum") {
    // f(z) = f(z) * z^2 + 1
    auto one = PolyGen::const_({Mod{1}});
    auto f = PolyGen::dummy();
    auto rhs = f.shift(2) + one;
    f.delegate(rhs);
    REQUIRE(f[1000000] == Mod{1});
    REQUIRE(f[1000001] == Mod{0});
  }

  SECTION("mul") {
    // (1 + z)^3 = 1 + 3 z + 3 z^2 + z^3
    auto f = PolyGen::const_({Mod{1}, Mod{1}});
    auto r = f * f * f;
    REQUIRE(r.slice(0, 5) ==
            std::vector<Mod>{Mod{1}, Mod{3}, Mod{3}, Mod{1}, Mod{0}});
  }
}
