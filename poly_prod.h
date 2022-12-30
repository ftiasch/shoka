#include "poly_op.h"

#include <queue>

template <typename Poly> struct PolyProduct : public PolyOp<Poly, PolyProduct> {
  using Base = PolyOp<Poly, PolyProduct>;
  SHOKA_HELPER_USING_POLY_OP;

  Poly operator()(const std::vector<Poly> &mons) const {
    if (mons.empty()) {
      return Poly{Mod{1}};
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
