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

static std::string rand_parenthesis(int n) {
  std::vector<bool> bits((n << 1) + 1);
  for (int i = 0; i <= n; ++i) {
    bits[i] = 1;
  }
  shuffle(bits.begin(), bits.end());
  int sum = 0;
  std::pair<int, int> best{0, 0};
  for (int i = 0; i <= n << 1; ++i) {
    sum += bits[i] ? -1 : 1;
    best = std::min(best, std::make_pair(sum, i + 1));
  }
  std::string result;
  for (int i = 0; i < n << 1; ++i) {
    result += bits[(best.second + i) % ((n << 1) + 1)] ? ')' : '(';
  }
  return result;
}
