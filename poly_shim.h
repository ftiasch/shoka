#include "ntt.h"
#include "poly.h"

#define DECLARE_POLY(MOD)                                                      \
  using NTT = ntt::NTT<Mod>;                                                   \
  using Factory = PolyFactory<NTT>;                                            \
  using Poly = Factory::Poly
