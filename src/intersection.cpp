#include "intersection.hpp"
#include <cassert>
#include <glm/gtx/intersect.hpp>
#include <vector>

namespace rainbow {

std::optional<RayTriangleIntersection> ComputeRayTriangleIntersection(
    const Ray& ray, const Triangle& triangle) {
  glm::vec2 barycentric_coordinates;
  float distance;
  if (glm::intersectRayTriangle(ray.origin, ray.direction, triangle.vertices[0],
                                triangle.vertices[1], triangle.vertices[2],
                                barycentric_coordinates, distance)) {
    return RayTriangleIntersection{
        ray.origin + distance * ray.direction, distance,
        glm::vec3{barycentric_coordinates, 1.0f - barycentric_coordinates.x -
                                               barycentric_coordinates.y}};
  } else {
    return {};
  }
}

bool CheckForTriangleAxisAlignedBoundingBoxIntersection(
    const Triangle& triangle, const AxisAlignedBoundingBox& aabb) {
  glm::vec3 triangle_normal =
      glm::normalize(glm::cross(triangle.vertices[1] - triangle.vertices[0],
                                triangle.vertices[2] - triangle.vertices[0]));

  float triangle_min, triangle_max;
  float box_min, box_max;

  // Test the box normals (x-, y- and z-axes)
  const glm::vec3 box_normals[]{glm::vec3(1, 0, 0), glm::vec3(0, 1, 0),
                                glm::vec3(0, 0, 1)};

  auto project = [](const auto& points, const glm::vec3& axis, float* min,
                    float* max) {
    assert(min != nullptr);
    assert(max != nullptr);
    *min = std::numeric_limits<float>::infinity();
    *max = -std::numeric_limits<float>::infinity();
    for (const auto& p : points) {
      float axis_dot_p = glm::dot(axis, p);
      if (axis_dot_p < *min) *min = axis_dot_p;
      if (axis_dot_p > *max) *max = axis_dot_p;
    }
  };

  for (int i = 0; i < 3; i++) {
    project(triangle.vertices, box_normals[i], &triangle_min, &triangle_max);
    if (triangle_max < aabb.min[i] || triangle_min > aabb.max[i]) {
      return false;  // No intersection possible.
    }
  }

  // Test the triangle normal
  float triangle_offset = glm::dot(triangle_normal, triangle.vertices[0]);
  std::vector<glm::vec3> box_vertices(8);
  for (int i = 0; i < 8; ++i) {
    glm::vec3 f((i / 4) % 2 == 0 ? 1 : -1, (i / 2) % 2 == 0 ? 1 : -1,
                i % 2 == 0 ? 1 : -1);
    box_vertices[i] =
        aabb.CalculateCenter() + 0.5f * f * aabb.CalculateExtend();
  }
  glm::vec3 normal = glm::cross(triangle.vertices[1] - triangle.vertices[0],
                                triangle.vertices[2] - triangle.vertices[0]);
  project(box_vertices, glm::normalize(normal), &box_min, &box_max);
  if (box_max < triangle_offset || box_min > triangle_offset)
    return false;  // No intersection possible.

  // Test the nine edge cross-products
  glm::vec3 triangle_edges[] = {triangle.vertices[0] - triangle.vertices[1],
                                triangle.vertices[1] - triangle.vertices[2],
                                triangle.vertices[2] - triangle.vertices[0]};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      // The box normals are the same as it's edge tangents
      glm::vec3 axis = glm::cross(triangle_edges[i], box_normals[j]);
      project(box_vertices, axis, &box_min, &box_max);
      project(triangle.vertices, axis, &triangle_min, &triangle_max);
      if (box_max < triangle_min || box_min > triangle_max)
        return false;  // No intersection possible
    }
  }

  // No separating axis found.
  return true;
}

}  // namespace rainbow
