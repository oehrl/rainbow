#include "rainbow/scene.hpp"

#include <cassert>
#include <chrono>
#include <iostream>

#include <assimp/postprocess.h>
#include <glm/gtx/string_cast.hpp>

#include "rainbow/integral_range.hpp"
#include "rainbow/octree.hpp"
#include "rainbow/random.hpp"
#include "rainbow/sampling.hpp"
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
    scene_ = importer_.ReadFile(
        filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
  };
  RAINBOW_TIME_SECTION("ConvertData") {
    materials_.resize(0);
    vertex_positions_.resize(0);
    triangles_.resize(0);
    total_flux_ = 0.0f;

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

      assert(mesh->HasPositions());
      vertex_positions_.reserve(vertex_positions_.size() + mesh->mNumVertices);
      assert(mesh->HasNormals());
      vertex_normals_.reserve(vertex_positions_.size() + mesh->mNumVertices);
      assert(mesh->HasFaces());
      triangles_.reserve(triangles_.size() + mesh->mNumFaces);

      for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
        vertex_positions_.push_back(
            ConvertAssimpVectorToGLM(mesh->mVertices[j]));
        vertex_normals_.push_back(ConvertAssimpVectorToGLM(mesh->mNormals[j]));
      }

      for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
        const auto& face = mesh->mFaces[j];
        assert(face.mNumIndices == 3);

        TriangleReference triangle{
            {base_vertex + face.mIndices[0], base_vertex + face.mIndices[1],
             base_vertex + face.mIndices[2]},
            mesh->mMaterialIndex};

        triangles_.push_back(triangle);

        const auto emissive_color =
            materials_[triangle.material_index].emissive_color;
        if (!IsBlack(emissive_color)) {
          emissive_triangles_.push_back(triangle);
          // TODO: calculate this correctly
          total_flux_ +=
              (emissive_color.x + emissive_color.y + emissive_color.z) *
              CalculateArea(ConstructTriangle(triangle));
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

  for (const auto& triangle_reference : triangles_) {
    const Triangle triangle = ConstructTriangle(triangle_reference);
    const auto intersection = ComputeRayTriangleIntersection(ray, triangle);
    if (intersection && intersection->distance >= 0.0f &&
        (!hitpoint || hitpoint->distance > intersection->distance)) {
      const Vector3 normals[3] = {
          vertex_normals_[triangle_reference.vertex_indices[0]],
          vertex_normals_[triangle_reference.vertex_indices[1]],
          vertex_normals_[triangle_reference.vertex_indices[2]]};
      const Vector3 normal =
          Normalize(normals[0] * intersection->barycentric_coordinates.x +
                    normals[1] * intersection->barycentric_coordinates.y +
                    normals[2] * intersection->barycentric_coordinates.z);

      hitpoint =
          HitPoint{intersection->distance, intersection->intersection_point,
                   normal, triangle_reference.material_index};
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
    float barycentric_coord_w =
        1.0f - barycentric_coord_u - barycentric_coord_v;

    const auto triangle_reference = emissive_triangles_[triangle_index];

    const auto triangle = ConstructTriangle(triangle_reference);

    const Vector3 photon_position = triangle.vertices[0] * barycentric_coord_u +
                                    triangle.vertices[1] * barycentric_coord_v +
                                    triangle.vertices[2] * barycentric_coord_w;

    const Vector3 normals[3] = {
        vertex_normals_[triangle_reference.vertex_indices[0]],
        vertex_normals_[triangle_reference.vertex_indices[1]],
        vertex_normals_[triangle_reference.vertex_indices[2]]};
    const Vector3 z = Normalize(normals[0] * barycentric_coord_u +
                                normals[1] * barycentric_coord_v +
                                normals[2] * barycentric_coord_w);
    const Vector3 x = ConstructOrthogonalVector(z);
    const Vector3 y = Cross(x, z);
    const Vector3 hemisphere_direction = SampleHemisphereCosineWeighted(
        real_distribution(default_random_number_engine),
        real_distribution(default_random_number_engine));
    const Vector3 photon_direction = x * hemisphere_direction.x +
                                     y * hemisphere_direction.y +
                                     z * hemisphere_direction.z;

    const Material& material =
        materials_[emissive_triangles_[triangle_index].material_index];

    photon_buffer->push_back(
        {photon_position, photon_direction, material.emissive_color});
  }
  assert(photon_buffer->size() == photon_count);
}

}  // namespace rainbow
