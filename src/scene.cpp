#include "rainbow/scene.hpp"

#include <cassert>
#include <chrono>
#include <iostream>

#include <assimp/postprocess.h>
#include <glm/gtx/string_cast.hpp>

#include "rainbow/octree.hpp"
#include "rainbow/timing.hpp"

namespace rainbow {

namespace {

inline glm::vec3 ConvertAssimpVectorToGLM(const aiVector3D& vector) {
  return {vector.x, vector.y, vector.z};
}

inline glm::vec4 ConvertAssimpColorToGLM(const aiColor4D& color) {
  return {color.r, color.g, color.b, color.a};
}

}  // namespace

Scene::Scene() : scene_{nullptr} {}

bool Scene::Load(const std::string& filename) {
  RAINBOW_TIME_SECTION("ReadFile()") {
    importer_.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
                                 aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    scene_ = importer_.ReadFile(filename, aiProcess_Triangulate |
                                              aiProcess_GenNormals |
                                              aiProcess_SortByPType);
  };
  RAINBOW_TIME_SECTION("ConvertData") {
    materials_.resize(0);
    vertices_.resize(0);
    triangles_.resize(0);

    materials_.reserve(scene_->mNumMaterials);
    for (unsigned int i = 0; i < scene_->mNumMaterials; ++i) {
      const auto& material = scene_->mMaterials[i];
      aiColor4D color;
      material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
      materials_.push_back({ConvertAssimpColorToGLM(color)});
    }

    for (unsigned int i = 0; i < scene_->mNumMeshes; ++i) {
      const auto mesh = scene_->mMeshes[i];

      const uint32_t base_vertex = vertices_.size();
      const uint32_t base_index = triangles_.size() * 3;
      const uint32_t triangle_count = mesh->mNumFaces;

      vertices_.reserve(vertices_.size() + mesh->mNumVertices);
      triangles_.reserve(triangles_.size() + mesh->mNumFaces);

      for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
        vertices_.push_back({ConvertAssimpVectorToGLM(mesh->mVertices[j])});
      }

      for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
        const auto& face = mesh->mFaces[j];
        assert(face.mNumIndices == 3);

        triangles_.push_back({{
                                  base_vertex + face.mIndices[0],
                                  base_vertex + face.mIndices[1],
                                  base_vertex + face.mIndices[2],
                              },
                              mesh->mMaterialIndex});
      }
    }
  };

  RAINBOW_TIME_SECTION("Compute octree") {
    octree_ =
        std::make_unique<Octree>(vertices_.data(), vertices_.size(), 6, 200);
    for (const auto& triangle : triangles_) {
      octree_->InsertTriangle(triangle);
    }
    octree_->Build();
  };
  std::cout << "Total number of triangles: " << GetTriangleCount() << std::endl;
  octree_->Print();

  RAINBOW_TIME_SECTION("Create OpenGL buffers") {
    ShaderStorageBufferDescription desc;

    desc.size = materials_.size() * sizeof(Material);
    material_buffer_ =
        std::make_unique<ShaderStorageBuffer>(desc, materials_.data());

    desc.size = vertices_.size() * sizeof(Vertex);
    vertex_buffer_ =
        std::make_unique<ShaderStorageBuffer>(desc, vertices_.data());

    desc.size = triangles_.size() * sizeof(Scene::Triangle);
    triangle_buffer_ =
        std::make_unique<ShaderStorageBuffer>(desc, triangles_.data());
  };

  return scene_ != nullptr;
}

std::optional<Scene::HitPoint> Scene::ShootRay(const Ray& ray) const {
  assert(scene_ != nullptr);

  std::optional<HitPoint> hitpoint;

  for (const auto& triangle : triangles_) {
    const auto intersection =
        ComputeRayTriangleIntersection(ray, GetTriangle(triangle));
    if (intersection && intersection->distance >= 0.0f &&
        (!hitpoint || hitpoint->distance > intersection->distance)) {
      hitpoint =
          HitPoint{intersection->distance, intersection->intersection_point,
                   glm::vec3{}, &materials_[triangle.material_index]};
    }
  }

  return hitpoint;
}

}  // namespace rainbow
