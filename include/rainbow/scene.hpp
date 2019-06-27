#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "rainbow/data_structures.hpp"
#include "rainbow/intersection.hpp"

namespace rainbow {

class Octree;

class Scene {
 public:
  struct HitPoint {
    float distance;
    Vector3 position;
    Vector3 normal;
    uint32_t material_index;
  };

  Scene();

  bool Load(const std::string& filename);

  std::optional<HitPoint> ShootRay(const Ray& ray) const;

  inline const std::vector<Material>& GetMaterials() const {
    return materials_;
  }

  inline const Material& GetMaterial(uint32_t index) const {
    return materials_[index];
  }

  inline const std::vector<Vector3>& GetVertexPositions() const {
    return vertex_positions_;
  }

  inline const std::vector<Vector3>& GetVertexNormals() const {
    return vertex_normals_;
  }

  inline const std::vector<TriangleReference>& GetTriangles() const {
    return triangles_;
  }

  inline uint32_t GetTriangleCount() const { return triangles_.size(); }
  inline float GetTotalFlux() const { return total_flux_; }

  inline Triangle ConstructTriangle(
      const TriangleReference& triangle_reference) const {
    return {
        vertex_positions_[triangle_reference.vertex_indices[0]],
        vertex_positions_[triangle_reference.vertex_indices[1]],
        vertex_positions_[triangle_reference.vertex_indices[2]],
    };
  }

  void GeneratePhotons(size_t photon_count,
                       std::vector<Photon>* photon_buffer) const;

 private:
  Assimp::Importer importer_;
  const aiScene* scene_;

  std::vector<Material> materials_;
  std::vector<Vector3> vertex_positions_;
  std::vector<Vector3> vertex_normals_;
  std::vector<TriangleReference> triangles_;
  std::vector<TriangleReference> emissive_triangles_;
  std::unique_ptr<Octree> octree_;
  float total_flux_;
};

}  // namespace rainbow

#include "rainbow/octree.hpp"
