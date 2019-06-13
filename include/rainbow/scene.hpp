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

#include "rainbow/intersection.hpp"

namespace rainbow {

class Octree;

class Scene {
 public:
  struct Material {
    glm::vec4 diffuse;
  };
  static_assert(sizeof(Material) == sizeof(float) * 4);

  struct Vertex {
    glm::vec3 position;
    float padding;
  };
  static_assert(sizeof(Vertex) == sizeof(float) * 4);

  struct Triangle {
    glm::uvec3 vertex_indices;
    uint32_t material_index;
  };
  static_assert(sizeof(Triangle) == sizeof(uint32_t) * 4);

  struct HitPoint {
    float distance;
    glm::vec3 position;
    glm::vec3 normal;
    const Material* material;
  };

  Scene();

  bool Load(const std::string& filename);

  std::optional<HitPoint> ShootRay(const Ray& ray) const;

  inline const std::vector<Material>& GetMaterials() const {
    return materials_;
  }

  inline const std::vector<Vertex>& GetVertices() const { return vertices_; }

  inline const std::vector<Triangle>& GetTriangles() const {
    return triangles_;
  }

  inline uint32_t GetTriangleCount() const { return triangles_.size(); }

  inline rainbow::Triangle GetTriangle(const Scene::Triangle& triangle) const {
    return {
        vertices_[triangle.vertex_indices[0]].position,
        vertices_[triangle.vertex_indices[1]].position,
        vertices_[triangle.vertex_indices[2]].position,
    };
  }

 private:
  Assimp::Importer importer_;
  const aiScene* scene_;

  std::vector<Material> materials_;
  std::vector<Vertex> vertices_;
  std::vector<Triangle> triangles_;
  std::unique_ptr<Octree> octree_;
};

}  // namespace rainbow

#include "rainbow/octree.hpp"
