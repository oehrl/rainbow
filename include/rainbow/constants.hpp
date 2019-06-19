#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

namespace rainbow {

template <typename T>
inline constexpr T Infinity() {
  return std::numeric_limits<T>::infinity();
}

template <typename T>
inline constexpr T Pi() {
  return T(M_PI);
}

template <typename T>
inline constexpr T TwoPi() {
  return 2 * Pi<T>();
}

}  // namespace rainbow