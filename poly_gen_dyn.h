#pragma once

#include "ntt.h"
#include "singleton.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

template <typename Mod> struct PolyGenDynT {
private:
  static constexpr int SHORT_ZEALOUS_THRESHOLD = 16;

  using Ntt = NttT<Mod, 0>;

  static Ntt &ntt() { return singleton<Ntt>(); }

  using Vector = std::vector<Mod>;

  struct Op;

  using PtrOp = std::shared_ptr<Op>;

  struct Op {
    explicit Op(int min_deg__, const std::string &name_, bool is_value_ = false)
        : min_deg_{min_deg__}, is_value{is_value_}, name{name_} {}

    virtual ~Op() = default;

    virtual Mod operator[](int) = 0;

    int min_deg() const { return min_deg_; }

    virtual int max_deg() const { return INT_MAX; }

    const std::string name;
    const bool is_value;

  private:
    const int min_deg_;
  };

  struct CachedOp : public Op {
    using Op::Op;

    Mod operator[](int i) override {
      while (computed() <= i) {
        if (hwm <= i) {
          throw std::logic_error("loop detected");
        }
        hwm = computed();
        // std::cerr << ">" << this->name << "[" << hwm << "]" << std::endl;
        compute_next();
        // std::cerr << "<" << this->name << "[" << hwm << "]=" << cache[hwm]
        //           << std::endl;
        hwm = INT_MAX;
      }
      return cache[i];
    }

  protected:
    virtual void compute_next() = 0;

    virtual int computed() const { return cache.size(); }

    Vector cache;

  private:
    int hwm = INT_MAX;
  };

  struct ModInvs : public CachedOp {
    explicit ModInvs() : CachedOp{1, "inv", true} {}

  private:
    using CachedOp::cache;

    void compute_next() override {
      int i = cache.size();
      if (i < 2) {
        cache.push_back(Mod{1});
      } else {
        cache.push_back(-Mod{Mod::mod() / i} * (*this)[Mod::mod() % i]);
      }
    }
  };

  static Mod mod_inv(int i) { return singleton<ModInvs>()[i]; }

  struct Value : public Op {
    // TODO: add more vector constructors
    explicit Value(const Vector &c_, const std::string &name_)
        : Op{get_min_deg(c_), name_, true}, c{c_} {}

    Mod operator[](int i) override {
      return i < static_cast<int>(c.size()) ? c[i] : Mod{0};
    }

    int max_deg() const override { return static_cast<int>(c.size()) - 1; }

  private:
    static int get_min_deg(const Vector &c) {
      int d = 0;
      while (d < static_cast<int>(c.size()) && c[d] == Mod{0}) {
        d++;
      }
      return d;
    }

    Vector c;
  };

  struct Add : public Op {
    explicit Add(PtrOp p_, PtrOp q_)
        : Op{std::min(p_->min_deg(), q_->min_deg()),
             '(' + p_->name + '+' + q_->name + ')',
             p_->is_value && q_->is_value},
          p{std::move(p_)}, q{std::move(q_)} {}

    Mod operator[](int i) override { return (*p)[i] + (*q)[i]; }

  private:
    PtrOp p, q;
  };

  struct Sub : public Op {
    explicit Sub(PtrOp p_, PtrOp q_)
        : Op{std::min(p_->min_deg(), q_->min_deg()),
             '(' + p_->name + '-' + q_->name + ')',
             p_->is_value && q_->is_value},
          p{std::move(p_)}, q{std::move(q_)} {}

    Mod operator[](int i) override { return (*p)[i] - (*q)[i]; }

  private:
    PtrOp p, q;
  };

  struct Shift : public Op {
    explicit Shift(PtrOp p_, int s_)
        : Op{p_->min_deg() + s_,
             '(' + p_->name + "*z^" + std::to_string(s_) + ')', p_->is_value},
          p{std::move(p_)}, s{s_} {}

    Mod operator[](int i) override { return i < s ? Mod{0} : (*p)[i - s]; }

  private:
    PtrOp p;
    int s;
  };

  struct Integral : public Op {
    explicit Integral(PtrOp p_)
        : Op{p_->min_deg() + 1, "int(" + p_->name = ")", p_->is_value},
          p{std::move(p_)} {}

    Mod operator[](int i) override {
      return i ? (*p)[i - 1] * mod_inv(i) : Mod{0};
    }

  private:
    PtrOp p;
  };

  struct ZealousConv : public CachedOp {
    explicit ZealousConv(PtrOp p_, PtrOp q_)
        : CachedOp{p_->min_deg() + q_->min_deg(),
                   '(' + p_->name + '*' + q_->name + ')'},
          p{std::move(p_)}, q{std::move(q_)} {}

  private:
    using CachedOp::cache;

    void compute_next() override {
      int k = cache.size();
      Mod result{0};
      for (int i = !!p->min_deg(); i <= k - !!q->min_deg(); ++i) {
        result += (*p)[i] * (*q)[k - i];
      }
      cache.push_back(result);
    }

    PtrOp p, q;
  };

  struct ShortZealousConv : public CachedOp {
    explicit ShortZealousConv(PtrOp p_, PtrOp q_)
        : CachedOp{p_->min_deg() + q_->min_deg(),
                   '(' + p_->name + "*_{short_semi}" + q_->name + ')'},
          p{std::move(p_)}, q{std::move(q_)} {}

  private:
    using CachedOp::cache;

    void compute_next() override {
      int k = cache.size();
      Mod result{0};
      for (int i = std::max<int>(!!p->min_deg(), k - q->max_deg());
           i <= k - !!q->min_deg(); ++i) {
        result += (*p)[i] * (*q)[k - i];
      }
      cache.push_back(result);
    }

    PtrOp p, q;
  };

  struct ConvBase : public CachedOp {
    using CachedOp::cache;

    explicit ConvBase(PtrOp p_, PtrOp q_, const std::string &conv_name)
        : CachedOp{p_->min_deg() + q_->min_deg(),
                   '(' + p_->name + "*_{" + conv_name + '}' + q_->name + ')'},
          p{std::move(p_)}, q{std::move(q_)} {
      cache.resize(1);
    }

  protected:
    void middle_product(int n, int pbegin, int pend, int qbegin, int qend) {
      copy_and_fill0(n, buffer.data(), p, pbegin, pend);
      copy_and_fill0(n, buffer1.data(), q, qbegin, qend);
      ntt().dif(n, buffer.data());
      ntt().dif(n, buffer1.data());
      auto inv_n = ntt().power_of_two_inv(n);
      for (int i = 0; i < n; ++i) {
        buffer[i] = inv_n * buffer[i] * buffer1[i];
      }
      ntt().dit(n, buffer.data());
    }

    PtrOp p, q;
    std::vector<Mod> buffer;

  private:
    static void copy_and_fill0(int n, Mod *dst, const PtrOp &p, int l, int r) {
      for (int i = 0; i < r - l; ++i) {
        dst[i] = (*p)[l + i];
      }
      std::fill(dst + (r - l), dst + n, Mod{0});
    }

    virtual void recur(int, int, int) = 0;

    int computed() const override { return computed_; }

    void compute_next() override {
      int next = computed_;
      if (next == cache.size()) {
        auto new_size = cache.size() << 1;
        ntt().reserve(new_size);
        cache.resize(new_size);
        buffer.resize(new_size);
        buffer1.resize(new_size);
      }
      recur(0, cache.size(), next);
      computed_++;
    }

    int computed_ = 0;
    std::vector<Mod> buffer1;
  };

  struct SemiConv : public ConvBase {
    explicit SemiConv(PtrOp p, PtrOp q)
        : ConvBase{std::move(p), std::move(q), "semi"} {}

  private:
    using ConvBase::cache, ConvBase::p, ConvBase::q, ConvBase::middle_product,
        ConvBase::buffer;

    void recur(int l, int r, int k) {
      if (l + 1 == r) {
        cache[l] += q->min_deg() ? Mod{0} : (*p)[l] * (*q)[0];
      } else {
        auto m = (l + r) >> 1;
        if (k < m) {
          recur(l, m, k);
        } else {
          if (k == m) {
            middle_product(r - l, l, m, 0, r - l);
            for (int i = m; i < r; ++i) {
              cache[i] += buffer[i - l];
            }
          }
          recur(m, r, k);
        }
      }
    }
  };

  struct FullConv : public ConvBase {
    explicit FullConv(PtrOp p, PtrOp q)
        : ConvBase{std::move(p), std::move(q), "full"} {}

  private:
    using ConvBase::cache, ConvBase::p, ConvBase::q, ConvBase::middle_product,
        ConvBase::buffer;

    void recur(int l, int r, int k) {
      if (l + 1 == r) {
        cache[l] += q->min_deg() ? Mod{0} : (*p)[l] * (*q)[0];
        cache[l] += !l || p->min_deg() ? Mod{0} : (*p)[0] * (*q)[l];
      } else {
        auto m = (l + r) >> 1;
        if (k < m) {
          recur(l, m, k);
        } else {
          if (k == m) {
            middle_product(r - l, l, m, 0, l ? r - l : m - l);
            for (int i = m; i < r; ++i) {
              cache[i] += buffer[i - l];
            }
            if (l) {
              middle_product(r - l, 0, r - l, l, m);
              for (int i = m; i < r; ++i) {
                cache[i] += buffer[i - l];
              }
            }
          }
          recur(m, r, k);
        }
      }
    }
  };

  struct Var;

  struct Wrapper final {
    explicit Wrapper(PtrOp op_) : op{std::move(op_)} {}

    Mod operator[](int i) { return (*op)[i]; }

    Wrapper shift(int s) const { return wrap<Shift>(op, s); }

    Wrapper integrate() const { return wrap<Integral>(op); }

    Wrapper operator+(const Wrapper &o) const { return wrap<Add>(op, o.op); }

    Wrapper operator-(const Wrapper &o) const { return wrap<Sub>(op, o.op); }

    Wrapper operator*(const Wrapper &o) const { return smart_conv(op, o.op); }

  private:
    friend struct Var;

    static Wrapper smart_conv(const PtrOp &p, const PtrOp &q) {
      if (p->is_value) {
        if (q->is_value) {
          throw std::logic_error("Value*Value not supported");
        }
        return smart_semi(q, p);
      } else {
        return q->is_value ? smart_semi(p, q) : wrap<FullConv>(p, q);
      }
    }

    static Wrapper smart_semi(const PtrOp &p, const PtrOp &q) {
      return q->max_deg() < SHORT_ZEALOUS_THRESHOLD
                 ? wrap<ShortZealousConv>(p, q)
                 : wrap<SemiConv>(p, q);
    }

    PtrOp op;
  };

  struct Var : public Op {
    explicit Var(const std::string &name_) : Op{0, name_, false} {}

    Mod operator[](int i) override { return (*p.lock())[i]; }

    void delegate(Wrapper w) { p = w.op; }

  private:
    std::weak_ptr<Op> p;
  };

  template <typename T, typename... Args> static Wrapper wrap(Args &&...args) {
    static_assert(std::is_base_of_v<Op, T>);
    return Wrapper{std::make_shared<T>(std::forward<Args>(args)...)};
  }

public:
  static std::pair<Wrapper, std::shared_ptr<Var>>
  var(const std::string &name = "") {
    auto v = std::make_shared<Var>(name);
    return {Wrapper{v}, v};
  }

  static Wrapper value(const Vector &c, const std::string &name = "") {
    return wrap<Value>(c, name);
  }
};
