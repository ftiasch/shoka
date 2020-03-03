template <typename Int> static std::set<Int> rand_subset(Int n, int k) {
  assert(n >= k);
  std::set<Int> result;
  for (Int i = n - k; i < n; ++i) {
    Int p = rnd.next(static_cast<Int>(0), i);
    if (result.count(p)) {
      result.insert(i);
    } else {
      result.insert(p);
    }
  }
  return set;
}
