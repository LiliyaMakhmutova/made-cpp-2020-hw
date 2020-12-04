#pragma once
#include <functional>

template <typename R, typename T>
constexpr std::function<R(T)> compose(const std::function<R(T)>& f) {
  return [=](T x) -> R {
    return f(std::forward<T>(x));
  };
}

template <typename R, typename T, typename... Fs>
constexpr decltype(auto) compose(const std::function<R(T)>& f, const Fs&... fs) {
  return [=](auto... x) -> decltype(auto) {
    return f(compose(fs...)(std::forward<decltype(x)>(x)...));
  };
}