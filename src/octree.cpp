#include "rainbow/octree.hpp"
#include <cmath>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <limits>
#include <map>

namespace rainbow {

Octree::Octree(const Vector3* vertices, size_t vertex_count, size_t max_depth,
               size_t triangles_per_cell)
    : vertices_(vertices),
      vertex_count_(vertex_count),
      max_depth_(max_depth),
      triangles_per_cell_(triangles_per_cell) {
  root_.depth = 0;
  root_.aabb.min = Vector3{std::numeric_limits<float>::infinity(),
                           std::numeric_limits<float>::infinity(),
                           std::numeric_limits<float>::infinity()};
  root_.aabb.max = Vector3{-std::numeric_limits<float>::infinity(),
                           -std::numeric_limits<float>::infinity(),
                           -std::numeric_limits<float>::infinity()};

  for (size_t i = 0; i < vertex_count; ++i) {
    root_.aabb.min.x = std::min(root_.aabb.min.x, vertices[i].x);
    root_.aabb.min.y = std::min(root_.aabb.min.y, vertices[i].y);
    root_.aabb.min.z = std::min(root_.aabb.min.z, vertices[i].z);
    root_.aabb.max.x = std::max(root_.aabb.max.x, vertices[i].x);
    root_.aabb.max.y = std::max(root_.aabb.max.y, vertices[i].y);
    root_.aabb.max.z = std::max(root_.aabb.max.z, vertices[i].z);
  }
}

void Octree::InsertTriangle(const TriangleReference& triangle_indices) {
  Triangle triangle = GetTriangle(triangle_indices);
  InsertTriangle(&root_, triangle_indices, triangle);
}

void Octree::Print() const { PrintCell(&root_); }

void Octree::InsertTriangle(OctreeCell* cell,
                            const TriangleReference& triangle_indices,
                            const Triangle& triangle) {
  assert(cell != nullptr);
  if (cell->children.size() > 0) {
    for (auto& child : cell->children) {
      InsertTriangle(&child, triangle_indices, triangle);
    }
  } else {
    if (CheckForTriangleAxisAlignedBoundingBoxIntersection(triangle,
                                                           cell->aabb)) {
      cell->triangles.push_back(triangle_indices);
      if (cell->triangles.size() > triangles_per_cell_ &&
          cell->depth < max_depth_) {
        SplitCell(cell);
      }
    }
  }
}

void Octree::Build() {
  data_.resize(0);
  triangles_.resize(0);

  uint32_t cell_count = 0;
  uint32_t triangle_count = 0;
  std::map<const OctreeCell*, int32_t> cell_ids;
  Traverse([&cell_count, &triangle_count, &cell_ids](const OctreeCell& cell,
                                                     const OctreeCell*) {
    cell_ids[&cell] = cell_count;
    ++cell_count;
    triangle_count += cell.triangles.size();
  });

  data_.reserve(cell_count);
  triangles_.reserve(triangle_count);

  Traverse([this, &cell_ids](const OctreeCell& cell, const OctreeCell* parent) {
    OctreeData data;
    data.aabb_min = cell.aabb.min;
    data.aabb_max = cell.aabb.max;
    data.triangles_begin = triangles_.size();
    data.triangles_end = triangles_.size() + cell.triangles.size();
    data.parent_index = parent == nullptr ? -1 : cell_ids[parent];
    if (cell.children.size() == 8) {
      for (int i = 0; i < 8; ++i) {
        data.child_indices[i] = cell_ids[&cell.children[i]];
      }
    } else {
      for (int i = 0; i < 8; ++i) {
        data.child_indices[i] = -1;
      }
    }
    data_.push_back(data);

    triangles_.insert(triangles_.end(), cell.triangles.begin(),
                      cell.triangles.end());
  });
}

void Octree::SplitCell(OctreeCell* cell) {
  assert(cell != nullptr);
  assert(cell->children.size() == 0);
  const Vector3 center = CalculateCenter(cell->aabb);
  const Vector3 children_extend = CalculateExtend(cell->aabb) * 0.5f;
  const Vector3 children_half_extend = children_extend * 0.5f;
  const Vector3 directions[8] = {Vector3{1, 1, 1},   Vector3{-1, 1, 1},
                                 Vector3{1, -1, 1},  Vector3{-1, -1, 1},
                                 Vector3{1, 1, -1},  Vector3{-1, 1, -1},
                                 Vector3{1, -1, -1}, Vector3{-1, -1, -1}};
  cell->children.reserve(8);
  for (const auto& direction : directions) {
    const Vector3 child_center = center + children_half_extend * direction;

    auto& child = cell->children.emplace_back();
    child.depth = cell->depth + 1;
    child.aabb.min = child_center - children_half_extend;
    child.aabb.max = child_center + children_half_extend;
    for (const auto& triangle_indices : cell->triangles) {
      InsertTriangle(&child, triangle_indices, GetTriangle(triangle_indices));
    }
  }
  cell->triangles.clear();
}

Triangle Octree::GetTriangle(const TriangleReference& triangle_indices) {
  return {vertices_[triangle_indices.vertex_indices[0]],
          vertices_[triangle_indices.vertex_indices[1]],
          vertices_[triangle_indices.vertex_indices[2]]};
}

void Octree::PrintCell(const OctreeCell* cell) const {
  auto indent = [](int n) {
    for (int i = 0; i < n; ++i) {
      std::cout << " ";
    }
  };

  indent(cell->depth);
  std::cout << "[" << cell->aabb.min << "-" << cell->aabb.max
            << "]: " << cell->triangles.size() << std::endl;

  if (cell->children.size() == 0) {
    for (const auto& triangle : cell->triangles) {
      // indent(cell->depth + 1);
      // std::cout << "["
      //           << glm::to_string(vertices_[triangle.indices[0]].position)
      //           << ","
      //           << glm::to_string(vertices_[triangle.indices[1]].position)
      //           << ","
      //           << glm::to_string(vertices_[triangle.indices[2]].position)
      //           << "]" << std::endl;
    }
  } else {
    for (const auto& child : cell->children) {
      PrintCell(&child);
    }
  }
}

size_t Octree::GetNumberOfTrianglesInChildren(const OctreeCell* cell) const {
  if (cell->children.size() > 0) {
    size_t triangle_count = 0;
    for (const auto& child : cell->children) {
      triangle_count += GetNumberOfTrianglesInChildren(&child);
    }
    return triangle_count;
  } else {
    return cell->triangles.size();
  }
}

}  // namespace rainbow
