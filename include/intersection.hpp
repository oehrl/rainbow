#pragma once

#include <glm/vec3.hpp>
#include <optional>

namespace rainbow {

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};

struct Triangle {
  glm::vec3 vertices[3];
};

struct RayTriangleIntersection {
  glm::vec3 intersection_point;
  float distance;
  glm::vec3 barycentric_coordinates;
};

std::optional<RayTriangleIntersection> ComputeRayTriangleIntersection(
    const Ray& ray, const Triangle& triangle);

}  // namespace rainbow