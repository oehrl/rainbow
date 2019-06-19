#include "rainbow/intersection.hpp"
#include <cassert>
#include <glm/gtx/intersect.hpp>
#include <vector>

namespace rainbow {

std::optional<RayTriangleIntersection> ComputeRayTriangleIntersection(
    const Ray& ray, const Triangle& triangle) {
  return {};
  // glm::vec2 barycentric_coordinates;
  // float distance;
  // if (glm::intersectRayTriangle(ray.origin, ray.direction,
  // triangle.vertices[0],
  //                               triangle.vertices[1], triangle.vertices[2],
  //                               barycentric_coordinates, distance)) {
  //   return RayTriangleIntersection{
  //       ray.origin + distance * ray.direction, distance,
  //       Vector3{barycentric_coordinates, 1.0f - barycentric_coordinates.x -
  //                                              barycentric_coordinates.y}};
  // } else {
  //   return {};
  // }
}

bool CheckForTriangleAxisAlignedBoundingBoxIntersection(
    const Triangle& triangle, const AxisAlignedBoundingBox& aabb) {
  auto project = [](const auto& points, const Vector3& axis, float* min,
                    float* max) {
    assert(min != nullptr);
    assert(max != nullptr);
    *min = std::numeric_limits<float>::infinity();
    *max = -std::numeric_limits<float>::infinity();
    for (const auto& p : points) {
      float axis_dot_p = Dot(axis, p);
      if (axis_dot_p < *min) *min = axis_dot_p;
      if (axis_dot_p > *max) *max = axis_dot_p;
    }
  };

  // Test the box normals (x-, y- and z-axes)
  const Vector3 aabb_normals[]{Vector3{1, 0, 0}, Vector3{0, 1, 0},
                               Vector3{0, 0, 1}};
  for (int i = 0; i < 3; i++) {
    float triangle_min, triangle_max;
    project(triangle.vertices, aabb_normals[i], &triangle_min, &triangle_max);
    if (triangle_max < aabb.min[i] || triangle_min > aabb.max[i]) {
      // No intersection possible.
      return false;
    }
  }

  // Test the triangle normal
  const Vector3 triangle_normal = CalculateNormal(triangle);
  const float triangle_offset = Dot(triangle_normal, triangle.vertices[0]);
  const auto aabb_vertices = CalculateVertexPositions(aabb);
  {
    float aabb_min, aabb_max;
    project(aabb_vertices, triangle_normal, &aabb_min, &aabb_max);
    if (aabb_max < triangle_offset || aabb_min > triangle_offset) {
      // No intersection possible.
      return false;
    }
  }

  // Test the nine edge cross-products
  const Vector3 triangle_edges[] = {
      triangle.vertices[0] - triangle.vertices[1],
      triangle.vertices[1] - triangle.vertices[2],
      triangle.vertices[2] - triangle.vertices[0],
  };
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      // The box normals are the same as it's edge tangents
      const Vector3 axis = Cross(triangle_edges[i], aabb_normals[j]);

      float aabb_min, aabb_max;
      project(aabb_vertices, axis, &aabb_min, &aabb_max);

      float triangle_min, triangle_max;
      project(triangle.vertices, axis, &triangle_min, &triangle_max);

      if (aabb_max < triangle_min || aabb_min > triangle_max) {
        // No intersection possible
        return false;
      }
    }
  }

  // No separating axis found.
  return true;
}

}  // namespace rainbow
