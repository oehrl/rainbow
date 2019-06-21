#include "rainbow/scene.hpp"

#include <cassert>
#include <chrono>
#include <iostream>

#include <assimp/postprocess.h>
#include <glm/gtx/string_cast.hpp>

#include "rainbow/integral_range.hpp"
#include "rainbow/octree.hpp"
#include "rainbow/random.hpp"
#include "rainbow/timing.hpp"

namespace rainbow {

namespace {

inline Vector3 ConvertAssimpVectorToGLM(const aiVector3D& vector) {
  return {vector.x, vector.y, vector.z};
}

inline Vector4 ConvertAssimpColorToGLM(const aiColor4D& color) {
  return {color.r, color.g, color.b, color.a};
}

inline bool IsBlack(const Vector4& color) {
  return color.x <= 0.0f && color.y <= 0.0f && color.z <= 0.0f;
}

}  // namespace

Scene::Scene() : scene_{nullptr} {}

bool Scene::Load(const std::string& filename) {
  RAINBOW_TIME_SECTION("ReadFile()") {
    importer_.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
                                 aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    scene_ = importer_.ReadFile(filename, aiProcess_Triangulate);
  };
  RAINBOW_TIME_SECTION("ConvertData") {
    materials_.resize(0);
    vertex_positions_.resize(0);
    triangles_.resize(0);

    materials_.reserve(scene_->mNumMaterials);
    for (unsigned int i = 0; i < scene_->mNumMaterials; ++i) {
      const auto& material = scene_->mMaterials[i];
      aiColor4D diffuse_color;
      aiColor4D emissive_color;
      material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color);
      material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive_color);
      materials_.push_back({ConvertAssimpColorToGLM(diffuse_color),
                            ConvertAssimpColorToGLM(emissive_color)});
    }

    for (unsigned int i = 0; i < scene_->mNumMeshes; ++i) {
      const auto mesh = scene_->mMeshes[i];

      const uint32_t base_vertex = vertex_positions_.size();
      const uint32_t base_index = triangles_.size() * 3;
      const uint32_t triangle_count = mesh->mNumFaces;

      vertex_positions_.reserve(vertex_positions_.size() + mesh->mNumVertices);
      triangles_.reserve(triangles_.size() + mesh->mNumFaces);

      for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
        vertex_positions_.push_back(
            {ConvertAssimpVectorToGLM(mesh->mVertices[j])});
      }

      for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
        const auto& face = mesh->mFaces[j];
        assert(face.mNumIndices == 3);

        TriangleReference triangle{
            {base_vertex + face.mIndices[0], base_vertex + face.mIndices[1],
             base_vertex + face.mIndices[2]},
            mesh->mMaterialIndex};

        triangles_.push_back(triangle);

        if (!IsBlack(materials_[triangle.material_index].emissive_color)) {
          emissive_triangles_.push_back(triangle);
          std::cout << triangle.vertex_indices[0] << ", "
                    << triangle.vertex_indices[1] << ", "
                    << triangle.vertex_indices[2] << "\n";
        }
      }
    }
  };

  RAINBOW_TIME_SECTION("Compute octree") {
    octree_ = std::make_unique<Octree>(vertex_positions_.data(),
                                       vertex_positions_.size(), 6, 200);
    for (const auto& triangle : triangles_) {
      octree_->InsertTriangle(triangle);
    }
    octree_->Build();
  };
  std::cout << "Total number of triangles: " << GetTriangleCount() << std::endl;
  octree_->Print();

  return scene_ != nullptr;
}

std::optional<Scene::HitPoint> Scene::ShootRay(const Ray& ray) const {
  assert(scene_ != nullptr);

  std::optional<HitPoint> hitpoint;

  for (const auto& triangle : triangles_) {
    const auto intersection =
        ComputeRayTriangleIntersection(ray, ConstructTriangle(triangle));
    if (intersection && intersection->distance >= 0.0f &&
        (!hitpoint || hitpoint->distance > intersection->distance)) {
      hitpoint =
          HitPoint{intersection->distance, intersection->intersection_point,
                   Vector3{}, &materials_[triangle.material_index]};
    }
  }

  return hitpoint;
}

void Scene::GeneratePhotons(size_t photon_count,
                            std::vector<Photon>* photon_buffer) const {
  assert(photon_buffer != nullptr);
  photon_buffer->resize(0);
  photon_buffer->reserve(photon_count);

  std::uniform_int_distribution triangle_index_distribution{
      static_cast<size_t>(0), emissive_triangles_.size() - 1};
  std::uniform_real_distribution<float> real_distribution;

  for (auto i : IntegralRange{photon_count}) {
    (void)i;
    const auto triangle_index =
        triangle_index_distribution(default_random_number_engine);
    const auto barycentric_coord_u =
        real_distribution(default_random_number_engine);
    const auto barycentric_coord_v =
        (1.0f - barycentric_coord_u) *
        real_distribution(default_random_number_engine);
    assert(barycentric_coord_u + barycentric_coord_v <= 1.0f);

    const auto triangle =
        ConstructTriangle(emissive_triangles_[triangle_index]);

    const Vector3 photon_position = {
        triangle.vertices[0] * barycentric_coord_u +
        triangle.vertices[1] * barycentric_coord_v +
        triangle.vertices[2] *
            (1.0f - barycentric_coord_u - barycentric_coord_v)};

    const Vector3 photon_direction = SampleHemisphereCosineWeighted(
        real_distribution(default_random_number_engine),
        real_distribution(default_random_number_engine));

    photon_buffer->push_back({photon_position, photon_direction});
  }
  assert(photon_buffer->size() == photon_count);
}

}  // namespace rainbow
