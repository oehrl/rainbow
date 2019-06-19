#pragma once

#include <glm/vec3.hpp>

#include "rainbow/data_structures.hpp"
#include "rainbow/intersection.hpp"
#include "rainbow/scene.hpp"

namespace rainbow {

class Octree {
 public:
  struct OctreeData {
    Vector3 aabb_min;
    Vector3 aabb_max;
    uint32_t triangles_begin;
    uint32_t triangles_end;
    int32_t parent_index;
    int32_t child_indices[8];
  };
  static_assert(sizeof(OctreeData) == 80);

  struct OctreeCell {
    size_t depth;
    AxisAlignedBoundingBox aabb;
    std::vector<TriangleReference> triangles;
    std::vector<OctreeCell> children;
  };
  Octree(const Vector3* vertex_positions, size_t vertex_count, size_t max_depth,
         size_t triangles_per_cell);

  void InsertTriangle(const TriangleReference& triangle);
  void Print() const;

  inline const std::vector<OctreeData>& GetOctreeBufferData() const {
    return data_;
  }
  const std::vector<TriangleReference>& GetTriangleBufferData() const {
    return triangles_;
  }
  void Build();

 private:
  const Vector3* vertices_;
  size_t vertex_count_;
  size_t max_depth_;
  size_t triangles_per_cell_;
  OctreeCell root_;
  std::vector<OctreeData> data_;
  std::vector<TriangleReference> triangles_;

  void InsertTriangle(OctreeCell* node,
                      const TriangleReference& triangle_indices,
                      const Triangle& triangle);
  void SplitCell(OctreeCell* node);
  Triangle GetTriangle(const TriangleReference& reference);
  void PrintCell(const OctreeCell* cell) const;
  size_t GetNumberOfTrianglesInChildren(const OctreeCell* cell) const;

  template <typename F>
  inline void Traverse(F&& function) {
    static_assert(std::is_invocable_v<F, const OctreeCell&, const OctreeCell*>);
    TraverseSubTree(root_, nullptr, function);
  }

  template <typename F>
  inline void TraverseSubTree(const OctreeCell& cell, const OctreeCell* parent,
                              F&& function) {
    static_assert(std::is_invocable_v<F, const OctreeCell&, const OctreeCell*>);
    function(cell, parent);
    for (const auto& child : cell.children) {
      TraverseSubTree(child, &cell, function);
    }
  }
};

}  // namespace rainbow
