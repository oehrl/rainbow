#pragma once

#include <cmath>
#include <ostream>
#include "rainbow/constants.hpp"

namespace rainbow {

union alignas(sizeof(float) * 2) Vector2 {
  struct {
    float x;
    float y;
  };
  float data[2];

  inline float& operator[](int index) { return data[index]; }
  inline float operator[](int index) const { return data[index]; }

  static constexpr int VECTOR_ELEMENT_COUNT = 2;
  inline static constexpr Vector2 Zero() { return {0.0f, 0.0f}; }
  inline static constexpr Vector2 One() { return {1.0f, 1.0f}; }
  inline static constexpr Vector2 Infinity() {
    return {rainbow::Infinity<float>(), rainbow::Infinity<float>()};
  }
  inline static constexpr Vector2 PositiveX() { return {1.0f, 0.0f}; }
  inline static constexpr Vector2 NegativeX() { return {-1.0f, 0.0f}; }
  inline static constexpr Vector2 PositiveY() { return {0.0f, 1.0f}; }
  inline static constexpr Vector2 NegativeY() { return {0.0f, -1.0f}; }
};
static_assert(sizeof(Vector2) == 8);

union alignas(sizeof(float) * 4) Vector3 {
  struct {
    float x;
    float y;
    float z;
  };
  float data[3];

  inline float& operator[](int index) { return data[index]; }
  inline float operator[](int index) const { return data[index]; }

  static constexpr int VECTOR_ELEMENT_COUNT = 3;
  inline static constexpr Vector3 Zero() { return {0.0f, 0.0f, 0.0f}; }
  inline static constexpr Vector3 One() { return {1.0f, 1.0f, 1.0f}; }
  inline static constexpr Vector3 Infinity() {
    return {rainbow::Infinity<float>(), rainbow::Infinity<float>(),
            rainbow::Infinity<float>()};
  }
  inline static constexpr Vector3 PositiveX() { return {1.0f, 0.0f, 0.0f}; }
  inline static constexpr Vector3 NegativeX() { return {-1.0f, 0.0f, 0.0f}; }
  inline static constexpr Vector3 PositiveY() { return {0.0f, 1.0f, 0.0f}; }
  inline static constexpr Vector3 NegativeY() { return {0.0f, -1.0f, 0.0f}; }
  inline static constexpr Vector3 PositiveZ() { return {0.0f, 0.0f, 1.0f}; }
  inline static constexpr Vector3 NegativeZ() { return {0.0f, 0.0f, -1.0f}; }
};
static_assert(sizeof(Vector3) == 16);

union alignas(sizeof(float) * 4) Vector4 {
  struct {
    float x;
    float y;
    float z;
    float w;
  };
  float data[4];

  inline float& operator[](int index) { return data[index]; }
  inline float operator[](int index) const { return data[index]; }

  static constexpr int VECTOR_ELEMENT_COUNT = 4;
  inline static constexpr Vector4 Zero() { return {0.0f, 0.0f, 0.0f, 0.0f}; }
  inline static constexpr Vector4 One() { return {1.0f, 1.0f, 1.0f, 1.0f}; }
  inline static constexpr Vector4 Infinity() {
    return {rainbow::Infinity<float>(), rainbow::Infinity<float>(),
            rainbow::Infinity<float>(), rainbow::Infinity<float>()};
  }
  inline static constexpr Vector4 PositiveX() {
    return {1.0f, 0.0f, 0.0f, 0.0f};
  }
  inline static constexpr Vector4 NegativeX() {
    return {-1.0f, 0.0f, 0.0f, 0.0f};
  }
  inline static constexpr Vector4 PositiveY() {
    return {0.0f, 1.0f, 0.0f, 0.0f};
  }
  inline static constexpr Vector4 NegativeY() {
    return {0.0f, -1.0f, 0.0f, 0.0f};
  }
  inline static constexpr Vector4 PositiveZ() {
    return {0.0f, 0.0f, 1.0f, 0.0f};
  }
  inline static constexpr Vector4 NegativeZ() {
    return {0.0f, 0.0f, -1.0f, 0.0f};
  }
  inline static constexpr Vector4 PositiveW() {
    return {0.0f, 0.0f, 0.0f, 1.0f};
  }
  inline static constexpr Vector4 NegativeW() {
    return {0.0f, 0.0f, 0.0f, -1.0f};
  }
};
static_assert(sizeof(Vector4) == 16);

template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline std::ostream& operator<<(std::ostream& lhs, const VectorType& rhs) {
  lhs.write("{", 1);
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    if (i > 0) {
      lhs.write(",", 1);
    }
    lhs << rhs[i];
  }
  lhs.write("}", 1);
  return lhs;
}

template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator+(const VectorType& lhs, const VectorType& rhs) {
  VectorType result;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result[i] = lhs[i] + rhs[i];
  }
  return result;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType& operator+=(VectorType& lhs, const VectorType& rhs) {
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    lhs[i] += rhs[i];
  }
  return lhs;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator-(const VectorType& rhs) {
  VectorType result;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result[i] = -rhs[i];
  }
  return result;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator-(const VectorType& lhs, const VectorType& rhs) {
  VectorType result;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result[i] = lhs[i] - rhs[i];
  }
  return result;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType& operator-=(VectorType& lhs, const VectorType& rhs) {
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    lhs[i] -= rhs[i];
  }
  return lhs;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator*(const VectorType& lhs, float rhs) {
  VectorType result;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result[i] = lhs[i] * rhs;
  }
  return result;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator*(float lhs, const VectorType& rhs) {
  VectorType result;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result[i] = lhs * rhs[i];
  }
  return result;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType& operator*=(VectorType& lhs, float rhs) {
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    lhs[i] *= rhs;
  }
  return lhs;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator*(const VectorType& lhs, const VectorType& rhs) {
  VectorType result;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result[i] = lhs[i] * rhs[i];
  }
  return result;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator*=(VectorType& lhs, const VectorType& rhs) {
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    lhs[i] *= rhs[i];
  }
  return lhs;
}

template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator/(const VectorType& lhs, float rhs) {
  VectorType result;
  const float inverse = 1.0f / rhs;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result[i] = lhs[i] * inverse;
  }
  return result;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator/(float lhs, const VectorType& rhs) {
  VectorType result;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result[i] = lhs / rhs[i];
  }
  return result;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType& operator/=(VectorType& lhs, float rhs) {
  const float inverse = 1.0f / rhs;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    lhs[i] *= inverse;
  }
  return lhs;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator/(const VectorType& lhs, const VectorType& rhs) {
  VectorType result;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result[i] = lhs[i] / rhs[i];
  }
  return result;
}
template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType operator/=(VectorType& lhs, const VectorType& rhs) {
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    lhs[i] /= rhs[i];
  }
  return lhs;
}

template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline float SquaredLength(const VectorType& vector) {
  float result = 0.0f;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result += vector[i] * vector[i];
  }
  return result;
}

template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline float Length(const VectorType& vector) {
  return std::sqrt(SquaredLength(vector));
}

template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline VectorType Normalize(const VectorType& vector) {
  return vector * (1.0f / Length(vector));
}

template <typename VectorType,
          int VECTOR_ELEMENT_COUNT = VectorType::VECTOR_ELEMENT_COUNT>
inline float Dot(const VectorType& lhs, const VectorType& rhs) {
  float result = 0.0f;
  for (int i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
    result += lhs[i] * rhs[i];
  }
  return result;
}

inline Vector3 Cross(const Vector3& lhs, const Vector3& rhs) {
  // clang-format off
  return {lhs.y * rhs.z - rhs.y * lhs.z,
		  lhs.z * rhs.x - rhs.z * lhs.x,
		  lhs.x * rhs.y - rhs.x * lhs.y};
  // clang-format on
}

inline Vector3 ConstructOrthogonalVector(const Vector3& vector) {
  if (std::abs(vector.x) > std::abs(vector.y)) {
    return Vector3{-vector.z, 0.0f, vector.x} /
           std::sqrt(vector.x * vector.x + vector.z * vector.z);
  } else {
    return Vector3{0.0f, vector.z, -vector.y} /
           std::sqrt(vector.y * vector.y + vector.z * vector.z);
  }
}

}  // namespace rainbow
