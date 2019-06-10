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

#include "buffer.hpp"
#include "intersection.hpp"
#include "opengl.hpp"

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

  inline ShaderStorageBuffer* GetMaterialBuffer() const {
    return material_buffer_.get();
  }

  inline ShaderStorageBuffer* GetVertexBuffer() const {
    return vertex_buffer_.get();
  }

  inline ShaderStorageBuffer* GetIndexBuffer() const {
    return index_buffer_.get();
  }

  inline ShaderStorageBuffer* GetMaterialIndexBuffer() const {
    return material_index_buffer_.get();
  }

 private:
  struct Mesh {
    uint32_t index_offset;
    uint32_t triangle_count;
    uint32_t material_index;
  };

  Assimp::Importer importer_;
  const aiScene* scene_;

  std::vector<Material> materials_;
  std::vector<glm::vec3> vertices_;
  std::vector<uint32_t> indices_;
  std::vector<uint32_t> material_indices_;

  std::unique_ptr<ShaderStorageBuffer> material_buffer_;
  std::unique_ptr<ShaderStorageBuffer> vertex_buffer_;
  std::unique_ptr<ShaderStorageBuffer> index_buffer_;
  std::unique_ptr<ShaderStorageBuffer> material_index_buffer_;
};

}  // namespace rainbow