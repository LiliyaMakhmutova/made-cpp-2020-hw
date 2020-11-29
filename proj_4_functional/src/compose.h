#pragma once

template <typename F>
class Function {
 public:
  Function(F f) : f(f) {}

  template <typename IN>
  decltype(auto) operator()(IN i) {
    return f(i);
  }

 private:
  F f;
};

template <typename F1, typename F2>
class Composition {
 public:
  Composition(F1 f1, F2 f2) : f1(f1), f2(f2) {}

  template <typename IN>
  decltype(auto) operator()(IN i) {
    return f2(f1(i));
  }

 private:
  F1 f1;
  F2 f2;
};

template <typename F>
decltype(auto) compose(F f) {
  return Function<F>{f};
}

template <typename F1, typename F2>
decltype(auto) compose(F1 f, F2 g) {
  return Composition<F1, F2>{f, g};
}

template <typename F, typename... Fs>
decltype(auto) compose(F f, Fs... args) {
  return compose(f, compose(args...));
}
