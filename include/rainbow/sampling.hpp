#pragma once

#include "rainbow/constants.hpp"
#include "rainbow/vector.hpp"

namespace rainbow {

inline Vector2 SampleDiskUniformly(float u, float v) {
  u = 2.0f * u - 1.0f;
  v = 2.0f * v - 1.0f;

  if (u == 0.0f && v == 0.0f) {
    return {0.0f, 0.0f};
  }

  float theta;
  float radius;
  if (std::abs(u) > std::abs(v)) {
    radius = u;
    theta = PiOverFour<float>() * (v / u);
  } else {
    radius = v;
    theta = PiOverTwo<float>() - PiOverFour<float>() * (u / v);
  }
  return {radius * std::cos(theta), radius * std::sin(theta)};
}

inline Vector3 SampleSphereUniformly(float u, float v) {
  const float phi = TwoPi<float>() * u;

  const float z = 1.0f - 2.0f * v;
  const float radius = std::sqrt(1.0f - z * z);
  return {radius * std::cos(phi), radius * std::sin(phi), z};
}

inline Vector3 SampleHemisphereUniformly(float u, float v) {
  const float phi = TwoPi<float>() * u;

  const float z = v;
  const float radius = std::sqrt(1.0f - z * z);
  return {radius * std::cos(phi), radius * std::sin(phi), z};
}

inline Vector3 SampleHemisphereCosineWeighted(float u, float v) {
  const Vector2 disk_sample = SampleDiskUniformly(u, v);
  const float z = std::sqrt(1.0f - SquaredLength(disk_sample));
  return {disk_sample.x, disk_sample.y, z};
}

}  // namespace rainbow