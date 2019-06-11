#pragma once

#include <cassert>
#include <glm/vec3.hpp>
#include <glm/vector_relational.hpp>
#include <optional>

namespace rainbow {

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};

struct Triangle {
  glm::vec3 vertices[3];
};

inline glm::vec3 CalculateCenter(const Triangle& triangle) {
  constexpr float ONE_THIRD = 1.0f / 3.0f;
  return ONE_THIRD *
         (triangle.vertices[0] + triangle.vertices[1] + triangle.vertices[2]);
}

struct AxisAlignedBoundingBox {
  glm::vec3 min;
  glm::vec3 max;
};

inline glm::vec3 CalculateExtend(const AxisAlignedBoundingBox& aabb) {
  assert(glm::all(glm::lessThanEqual(aabb.min, aabb.max)));
  return aabb.max - aabb.min;
}

inline glm::vec3 CalculateCenter(const AxisAlignedBoundingBox& aabb) {
  assert(glm::all(glm::lessThanEqual(aabb.min, aabb.max)));
  return (aabb.min + aabb.max) * 0.5f;
}

struct RayTriangleIntersection {
  glm::vec3 intersection_point;
  float distance;
  glm::vec3 barycentric_coordinates;
};

std::optional<RayTriangleIntersection> ComputeRayTriangleIntersection(
    const Ray& ray, const Triangle& triangle);

bool CheckForTriangleAxisAlignedBoundingBoxIntersection(
    const Triangle& triangle, const AxisAlignedBoundingBox& aabb);

}  // namespace rainbow
