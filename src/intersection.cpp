#include "rainbow/intersection.hpp"
#include <cassert>
#include <vector>

namespace rainbow {

std::optional<RayTriangleIntersection> ComputeRayTriangleIntersection(
    const Ray& ray, const Triangle& triangle) {
  const auto v0v1 = triangle.vertices[1] - triangle.vertices[0];
  const auto v0v2 = triangle.vertices[2] - triangle.vertices[0];
  const auto p_vector = Cross(ray.direction, v0v2);
  const float determinant = Dot(v0v1, p_vector);
  if (std::abs(determinant) <= std::numeric_limits<float>::epsilon()) {
    return {};
  }
  const float inverse_determinant = 1.0f / determinant;
  const Vector3 t_vector = ray.origin - triangle.vertices[0];
  const float u = inverse_determinant * Dot(t_vector, p_vector);
  if (u < 0.0f || u > 1.0f) {
    return {};
  }
  const Vector3 q_vector = Cross(t_vector, v0v1);
  const float v = inverse_determinant * Dot(ray.direction, q_vector);
  if (v < 0.0f || u + v > 1.0f) {
    return {};
  }
  const float t = Dot(v0v2, q_vector) / inverse_determinant;
  if (t < 0.0f) {
    return {};
  }

  return RayTriangleIntersection{
      ray.origin + t * ray.direction, {u, v, 1.0f - u - v}, t};
}

bool CheckForTriangleAxisAlignedBoundingBoxIntersection(
    const Triangle& triangle, const AxisAlignedBoundingBox& aabb) {
  auto project = [](const auto& points, const Vector3& axis, float* min,
                    float* max) {
    assert(min != nullptr);
    assert(max != nullptr);
    *min = Infinity<float>();
    *max = -Infinity<float>();
    for (const auto& p : points) {
      float axis_dot_p = Dot(axis, p);
      if (axis_dot_p < *min) *min = axis_dot_p;
      if (axis_dot_p > *max) *max = axis_dot_p;
    }
  };

  // Test the box normals (x-, y- and z-axes)
  // clang-format off
  const Vector3 aabb_normals[]{
    Vector3::PositiveX(),
    Vector3::PositiveY(),
    Vector3::PositiveZ()};
  // clang-format on
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
