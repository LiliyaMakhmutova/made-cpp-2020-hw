#include <functional>
#include <iostream>
#include "compose.h"

void FailWithMsg(const std::string& msg, int line) {
  std::cerr << "Test failed!\n";
  std::cerr << "[Line " << line << "] " << msg << std::endl;
  std::exit(EXIT_FAILURE);
}

#define ASSERT_TRUE(cond)                              \
  if (!(cond)) {                                       \
    FailWithMsg("Assertion failed: " #cond, __LINE__); \
  };


int main() {
  const std::function<bool(int)> a = [](int x) -> bool {
    return x > 10;
  };

  const std::function<int(float)> b = [](float y) -> int {
    return int(y * y);
  };

  const std::function<float(bool)> c = [](bool z) -> float {
    return z ? 3.1f : 3.34f;
  };

  const std::function<int(int)> inc = [](int z) -> int {
    return z + 1;
  };

  {
    auto d = compose(inc);
    auto val = 1;
    ASSERT_TRUE(d(val) == inc(val));
  }

  {
    auto d = compose(inc, inc);
    auto val = 1;
    ASSERT_TRUE(d(val) == inc(inc(val)));
  }

  {
    auto d = compose(a, inc);
    auto val = 1;
    ASSERT_TRUE(d(val) == inc(a(val)));
  }

  {
    auto d = compose(a, b, c, a, b, c);
    auto val = true;
    ASSERT_TRUE(d(val) == c(b(a(c(b(a(val)))))));
  }
  
  return 0;
}

