#pragma once
#include <functional>

template <typename R, typename T>
constexpr decltype(auto) compose(std::function<R(T)> f) {
  return [=](auto&&... x) -> decltype(auto) {
    return f(std::forward<decltype(x)>(x)...);
  };
}

template <typename R, typename T, typename... Fs>
constexpr decltype(auto) compose(std::function<R(T)> f, Fs... fs) {
  return [=](auto&&... x) -> decltype(auto) {
    return f(compose(fs...)(std::forward<decltype(x)>(x)...));
  };
}