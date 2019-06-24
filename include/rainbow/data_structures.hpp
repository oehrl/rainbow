#pragma once

#include <cstdint>
#include "rainbow/vector.hpp"

namespace rainbow {

struct Material {
  Vector4 diffuse_color;
  Vector4 emissive_color;
};
static_assert(sizeof(Material) == 32);

struct TriangleReference {
  uint32_t vertex_indices[3];
  uint32_t material_index;
};
static_assert(sizeof(TriangleReference) == 16);

struct Hitpoint {
  Vector3 position;
  Vector3 normal;
  Vector3 direction;
  Vector4 radiance_estimate;
  uint32_t pixel_location[2];
  uint32_t material_index;
  float radius;
};

struct Photon {
  Vector3 position;
  Vector3 direction;
  Vector4 color;
};

}  // namespace rainbow