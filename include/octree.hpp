#pragma once

#include <glm/vec3.hpp>

#include "intersection.hpp"
#include "scene.hpp"

namespace rainbow {

class Octree {
 public:
  struct OctreeData {};

  struct OctreeCell {
    size_t depth;
    AxisAlignedBoundingBox aabb;
    std::vector<Scene::Triangle> triangles;
    std::vector<OctreeCell> children;
  };
  Octree(const Scene::Vertex* vertices, size_t vertex_count, size_t max_depth,
         size_t triangles_per_cell);

  void InsertTriangle(const Scene::Triangle& triangle);
  void Print() const;

 private:
  const Scene::Vertex* vertices_;
  size_t vertex_count_;
  size_t max_depth_;
  size_t triangles_per_cell_;
  OctreeCell root_;

  void InsertTriangle(OctreeCell* node, const Scene::Triangle& triangle_indices,
                      const Triangle& triangle);
  void SplitCell(OctreeCell* node);
  Triangle GetTriangleFromTriangleIndices(
      const Scene::Triangle& triangle_indices);
  void PrintCell(const OctreeCell* cell) const;
  size_t GetNumberOfTrianglesInChildren(const OctreeCell* cell) const;
};

}  // namespace rainbow
