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

#include "intersection.hpp"

namespace rainbow {

class Scene {
 public:
  struct Material {
    glm::vec3 diffuse;
  };

  struct HitPoint {
    float distance;
    glm::vec3 position;
    glm::vec3 normal;
    const Material* material;
  };

  Scene();

  bool Load(const std::string& filename);

  std::optional<HitPoint> ShootRay(const Ray& ray) const;

 private:
  struct Mesh {
    uint32_t index_offset;
    uint32_t triangle_count;
    uint32_t material_index;
  };

  Assimp::Importer importer_;
  const aiScene* scene_;

  std::vector<glm::vec3> vertices_;
  std::vector<uint32_t> indices_;
  std::vector<Material> materials_;
  std::vector<Mesh> meshes_;
};

}  // namespace rainbow