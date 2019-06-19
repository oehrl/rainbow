#pragma once

#include <cmath>
#include <ostream>

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
  return std::sqrtf(SquaredLength(vector));
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
  // TODO: actually implement this
  return {0.0f, 0.0f, 0.0f};
}

}  // namespace rainbow
