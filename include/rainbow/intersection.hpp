#pragma once

#include <array>
#include <cassert>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vector_relational.hpp>
#include <optional>
#include "rainbow/constants.hpp"
#include "rainbow/vector.hpp"

namespace rainbow {

struct Ray {
  Vector3 origin;
  Vector3 direction;
};

struct Triangle {
  Vector3 vertices[3];
};

inline Vector3 CalculateCenter(const Triangle& triangle) {
  constexpr float ONE_THIRD = 1.0f / 3.0f;
  return ONE_THIRD *
         (triangle.vertices[0] + triangle.vertices[1] + triangle.vertices[2]);
}

inline Vector3 CalculateNormal(const Triangle& triangle) {
  return Normalize(Cross(triangle.vertices[1] - triangle.vertices[0],
                         triangle.vertices[2] - triangle.vertices[0]));
}

struct AxisAlignedBoundingBox {
  Vector3 min;
  Vector3 max;
};

inline Vector3 CalculateExtend(const AxisAlignedBoundingBox& aabb) {
  // assert(glm::all(glm::lessThanEqual(aabb.min, aabb.max)));
  return aabb.max - aabb.min;
}

inline Vector3 CalculateHalfExtend(const AxisAlignedBoundingBox& aabb) {
  // assert(glm::all(glm::lessThanEqual(aabb.min, aabb.max)));
  return CalculateExtend(aabb) * 0.5f;
}

inline Vector3 CalculateCenter(const AxisAlignedBoundingBox& aabb) {
  // assert(glm::all(glm::lessThanEqual(aabb.min, aabb.max)));
  return (aabb.min + aabb.max) * 0.5f;
}

inline std::array<Vector3, 8> CalculateVertexPositions(
    const AxisAlignedBoundingBox& aabb) {
  // assert(glm::all(glm::lessThanEqual(aabb.min, aabb.max)));
  const auto center = CalculateCenter(aabb);
  const auto half_extend = CalculateHalfExtend(aabb);

  // clang-format off
  return {
    center + (Vector3{-1.0f, -1.0f, -1.0f} * half_extend),
    center + (Vector3{-1.0f, -1.0f,  1.0f} * half_extend),
    center + (Vector3{-1.0f,  1.0f, -1.0f} * half_extend),
    center + (Vector3{-1.0f,  1.0f,  1.0f} * half_extend),
    center + (Vector3{ 1.0f, -1.0f, -1.0f} * half_extend),
    center + (Vector3{ 1.0f, -1.0f,  1.0f} * half_extend),
    center + (Vector3{ 1.0f,  1.0f, -1.0f} * half_extend),
    center + (Vector3{ 1.0f,  1.0f,  1.0f} * half_extend)
  };
  // clang-format on
}

struct RayTriangleIntersection {
  Vector3 intersection_point;
  Vector3 barycentric_coordinates;
  float distance;
};

std::optional<RayTriangleIntersection> ComputeRayTriangleIntersection(
    const Ray& ray, const Triangle& triangle);

bool CheckForTriangleAxisAlignedBoundingBoxIntersection(
    const Triangle& triangle, const AxisAlignedBoundingBox& aabb);

}  // namespace rainbow
