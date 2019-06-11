#pragma once

#include <array>
#include <cassert>
#include <glm/geometric.hpp>
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

inline glm::vec3 CalculateNormal(const Triangle& triangle) {
  return glm::normalize(
      glm::cross(triangle.vertices[1] - triangle.vertices[0],
                 triangle.vertices[2] - triangle.vertices[0]));
}

struct AxisAlignedBoundingBox {
  glm::vec3 min;
  glm::vec3 max;
};

inline glm::vec3 CalculateExtend(const AxisAlignedBoundingBox& aabb) {
  assert(glm::all(glm::lessThanEqual(aabb.min, aabb.max)));
  return aabb.max - aabb.min;
}

inline glm::vec3 CalculateHalfExtend(const AxisAlignedBoundingBox& aabb) {
  assert(glm::all(glm::lessThanEqual(aabb.min, aabb.max)));
  return CalculateExtend(aabb) * 0.5f;
}

inline glm::vec3 CalculateCenter(const AxisAlignedBoundingBox& aabb) {
  assert(glm::all(glm::lessThanEqual(aabb.min, aabb.max)));
  return (aabb.min + aabb.max) * 0.5f;
}

inline std::array<glm::vec3, 8> CalculateVertexPositions(
    const AxisAlignedBoundingBox& aabb) {
  assert(glm::all(glm::lessThanEqual(aabb.min, aabb.max)));
  const auto center = CalculateCenter(aabb);
  const auto half_extend = CalculateHalfExtend(aabb);

  // clang-format off
  return {
    center + (glm::vec3{-1.0f, -1.0f, -1.0f} * half_extend),
    center + (glm::vec3{-1.0f, -1.0f,  1.0f} * half_extend),
    center + (glm::vec3{-1.0f,  1.0f, -1.0f} * half_extend),
    center + (glm::vec3{-1.0f,  1.0f,  1.0f} * half_extend),
    center + (glm::vec3{ 1.0f, -1.0f, -1.0f} * half_extend),
    center + (glm::vec3{ 1.0f, -1.0f,  1.0f} * half_extend),
    center + (glm::vec3{ 1.0f,  1.0f, -1.0f} * half_extend),
    center + (glm::vec3{ 1.0f,  1.0f,  1.0f} * half_extend)
  };
  // clang-format on
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
