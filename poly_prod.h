#include "poly_op.h"

#include <queue>

template <typename NTT> struct PolyProduct : public PolyOp<PolyProduct, NTT> {
  using Base = PolyOp<PolyProduct, NTT>;
  using Base::factory;
  using typename Base::Factory;
  using typename Base::Mod;
  using typename Base::Poly;

  explicit PolyProduct(std::shared_ptr<Factory> factory_) : Base{factory_} {}

  Poly operator()(const std::vector<Poly> &mons) const {
    if (mons.empty()) {
      return factory->make(std::vector<Mod>{Mod{1}});
    }
    std::priority_queue<Poly, std::vector<Poly>, ByDeg> pq;
    for (auto &&mon : mons) {
      pq.emplace(mon);
    }
    while (pq.size() > 1) {
      auto p = pq.top();
      pq.pop();
      auto q = pq.top();
      pq.pop();
      pq.push(p * q);
    }
    return pq.top();
  }

private:
  struct ByDeg {
    bool operator()(const Poly &p, const Poly &q) { return p.deg() > q.deg(); }
  };
};
