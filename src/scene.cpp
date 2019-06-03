#include "scene.hpp"

#include <cassert>
#include <iostream>

#include <assimp/postprocess.h>
#include <glm/gtx/string_cast.hpp>

namespace rainbow {

namespace {

inline glm::vec3 ConvertAssimpVectorToGLM(const aiVector3D& vector) {
  return {vector.x, vector.y, vector.z};
}

}  // namespace

Scene::Scene() : scene_{nullptr} {}

bool Scene::Load(const std::string& filename) {
  scene_ = importer_.ReadFile(filename,
                              aiProcess_Triangulate | aiProcess_GenNormals);
  return IsValid();
}

std::optional<Scene::HitPoint> Scene::ShootRay(const Ray& ray) const {
  assert(IsValid());

  std::optional<HitPoint> hitpoint;

  for (unsigned int i = 0; i < scene_->mNumMeshes; ++i) {
    const auto mesh = scene_->mMeshes[i];
    for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
      const auto& face = mesh->mFaces[j];
      assert(face.mNumIndices == 3);

      Triangle triangle;
      glm::vec3 normals[3];
      for (int k = 0; k < 3; ++k) {
        assert(face.mIndices[k] < mesh->mNumVertices);
        triangle.vertices[k] =
            ConvertAssimpVectorToGLM(mesh->mVertices[face.mIndices[k]]);
        normals[k] = ConvertAssimpVectorToGLM(mesh->mNormals[face.mIndices[k]]);
      }

      const auto intersection = ComputeRayTriangleIntersection(ray, triangle);
      if (intersection && intersection->distance >= 0.0f &&
          (!hitpoint || hitpoint->distance > intersection->distance)) {
        const auto normal =
            normals[0] * intersection->barycentric_coordinates.x +
            normals[1] * intersection->barycentric_coordinates.y +
            normals[2] * intersection->barycentric_coordinates.z;

        hitpoint =
            HitPoint{intersection->distance, intersection->intersection_point,
                     normal, scene_->mMaterials[mesh->mMaterialIndex]};
      }
    }
  }

  return hitpoint;
}

}  // namespace rainbow
