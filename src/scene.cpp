#include "scene.hpp"

#include <cassert>
#include <chrono>
#include <iostream>

#include <assimp/postprocess.h>
#include <glm/gtx/string_cast.hpp>

#include "timing.hpp"

namespace rainbow {

namespace {

inline glm::vec3 ConvertAssimpVectorToGLM(const aiVector3D& vector) {
  return {vector.x, vector.y, vector.z};
}

inline glm::vec3 ConvertAssimpColorToGLM(const aiColor3D& color) {
  return {color.r, color.g, color.b};
}

}  // namespace

Scene::Scene() : scene_{nullptr} {}

bool Scene::Load(const std::string& filename) {
  RAINBOW_TIME_SECTION("ReadFile()") {
    scene_ = importer_.ReadFile(filename,
                                aiProcess_Triangulate | aiProcess_GenNormals);
  };
  RAINBOW_TIME_SECTION("ConvertData") {
    materials_.resize(0);
    vertices_.resize(0);
    indices_.resize(0);
    material_indices_.resize(0);

    materials_.reserve(scene_->mNumMaterials);
    for (unsigned int i = 0; i < scene_->mNumMaterials; ++i) {
      const auto& material = scene_->mMaterials[i];
      aiColor3D color;
      material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
      materials_.push_back({ConvertAssimpColorToGLM(color)});
    }

    for (unsigned int i = 0; i < scene_->mNumMeshes; ++i) {
      const auto mesh = scene_->mMeshes[i];

      const uint32_t base_vertex = vertices_.size();
      const uint32_t base_index = indices_.size();
      const uint32_t triangle_count = mesh->mNumFaces;

      vertices_.reserve(vertices_.size() + mesh->mNumVertices);
      indices_.reserve(indices_.size() + mesh->mNumFaces * 3);
      material_indices_.reserve(material_indices_.size() + mesh->mNumFaces);

      for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
        vertices_.push_back(ConvertAssimpVectorToGLM(mesh->mVertices[j]));
      }

      for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
        const auto& face = mesh->mFaces[j];
        assert(face.mNumIndices == 3);

        for (int k = 0; k < 3; ++k) {
          indices_.push_back(base_vertex + face.mIndices[k]);
        }
      }

      material_indices_.insert(material_indices_.end(), mesh->mNumFaces,
                               mesh->mMaterialIndex);
    }
  };

  RAINBOW_TIME_SECTION("Create OpenGL buffers") {
    ShaderStorageBufferDescription desc;

    desc.size = materials_.size() * sizeof(glm::vec3);
    material_buffer_ =
        std::make_unique<ShaderStorageBuffer>(desc, materials_.data());

    desc.size = vertices_.size() * sizeof(glm::vec3);
    vertex_buffer_ =
        std::make_unique<ShaderStorageBuffer>(desc, vertices_.data());

    desc.size = indices_.size() * sizeof(uint32_t);
    index_buffer_ =
        std::make_unique<ShaderStorageBuffer>(desc, indices_.data());

    desc.size = material_indices_.size() * sizeof(uint32_t);
    material_index_buffer_ =
        std::make_unique<ShaderStorageBuffer>(desc, material_indices_.data());
  };

  return scene_ != nullptr;
}

std::optional<Scene::HitPoint> Scene::ShootRay(const Ray& ray) const {
  assert(scene_ != nullptr);

  std::optional<HitPoint> hitpoint;

  assert(indices_.size() % 3 == 0);
  const uint32_t triangle_count = indices_.size() / 3;
  for (uint32_t i = 0; i < triangle_count; ++i) {
    Triangle triangle{
        vertices_[indices_[i * 3 + 0]],
        vertices_[indices_[i * 3 + 1]],
        vertices_[indices_[i * 3 + 2]],
    };

    const auto intersection = ComputeRayTriangleIntersection(ray, triangle);
    if (intersection && intersection->distance >= 0.0f &&
        (!hitpoint || hitpoint->distance > intersection->distance)) {
      hitpoint =
          HitPoint{intersection->distance, intersection->intersection_point,
                   glm::vec3{}, &materials_[material_indices_[i]]};
    }
  }

  return hitpoint;
}

}  // namespace rainbow
