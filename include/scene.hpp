#pragma once

#include <memory>
#include <optional>
#include <string>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <glm/vec3.hpp>

#include "intersection.hpp"

namespace rainbow {

class Scene {
 public:
  struct HitPoint {
    float distance;
    glm::vec3 position;
    glm::vec3 normal;
    aiMaterial* material;
  };

  Scene();

  bool Load(const std::string& filename);
  inline bool IsValid() const { return scene_ != nullptr; }

  std::optional<HitPoint> ShootRay(const Ray& ray) const;

 private:
  Assimp::Importer importer_;
  const aiScene* scene_;
};

}  // namespace rainbow