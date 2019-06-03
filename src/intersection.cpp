#include "intersection.hpp"
#include <glm/gtx/intersect.hpp>

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

}  // namespace rainbow
