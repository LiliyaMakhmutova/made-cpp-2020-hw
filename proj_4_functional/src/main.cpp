#include <functional>
#include <iostream>

#include "compose.h"

int main() {
  const std::function<bool(int)> a = [](int x) -> bool { return x > 10; };

  const std::function<int(float)> b = [](float y) -> int { return int(y * y); };

  const std::function<float(bool)> c = [](bool z) -> float {
    return z ? 3.1f : 3.34f;
  };

  const std::function<int(int)> inc = [](int z) -> int { return z + 1; };

  constexpr int val = 1;

  {
    constexpr auto d = compose(inc);
    static_assert(d(val) == 1, "Error when one function (increment) passed");
  }

  {
    constexpr auto d = compose(inc, inc);
    constexpr int val = 1;
    static_assert(d(val) == 2, "Error when two functions (increment) passed");
  }

  {
    constexpr auto d = compose(a, inc);
    constexpr int val = 1;
    static_assert(d(val) == 0,
                  "Error when two functions (increment and a) passed");
  }

  {
    constexpr auto d = compose(a, b, c, a, b, c);
    constexpr int val = 1;
    static_assert(d(val) == 1, "Error when six functions (a,b,c,a,b,c) passed");
  }

  return 0;
}
