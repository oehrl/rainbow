#include "rainbow/backends/cpu/cpu_backend.hpp"
#include <glm/vec3.hpp>
#include <iostream>
#include "rainbow/camera.hpp"
#include "rainbow/parallel.hpp"
#include "rainbow/scene.hpp"
#include "rainbow/timing.hpp"
#include "rainbow/viewport.hpp"

namespace rainbow {

void CPUBackend::Prepare(const Scene& scene, size_t viewport_width,
                         size_t viewport_height) {
  scene_ = &scene;
}

void CPUBackend::Render(const Camera& camera, Viewport* viewport) {
  const size_t viewport_width = viewport->GetWidth();
  const size_t viewport_height = viewport->GetHeight();

  GenerateHitpoints(camera, viewport_width, viewport_height);

  RAINBOW_TIME_SECTION("Photon Generation") {
    scene_->GeneratePhotons(1000, &photon_buffer_);
  };

  RAINBOW_TIME_SECTION("Photon Tracing") {
    for (auto& photon : photon_buffer_) {
      auto intersection = scene_->ShootRay({photon.position, photon.direction});
      if (intersection) {
        photon.position = intersection->position;
      }
    }
  };

  EvaluateRadiance(viewport);
}

void CPUBackend::GenerateHitpoints(const Camera& camera, size_t viewport_width,
                                   size_t viewport_height) {
  RAINBOW_TIME_FUNCTION();

  Vector3 right;
  Vector3 up;
  Vector3 forward;
  camera.GetAxisVectors(&right, &up, &forward);

  hitpoints_.resize(0);
  hitpoints_.reserve(viewport_width * viewport_height);
  ParallelFor(viewport_height, [&](size_t y) {
    const float y_normalized =
        static_cast<float>(y) / (viewport_height - 1) - 0.5f;
    for (size_t x = 0; x < viewport_width; ++x) {
      const float x_normalized =
          static_cast<float>(x) / (viewport_width - 1) - 0.5f;

      const auto view_ray_direction =
          Normalize(x_normalized * right + y_normalized * up + forward);

      const auto hitpoint =
          scene_->ShootRay({camera.GetPosition(), view_ray_direction});

      if (hitpoint) {
        std::unique_lock lock{hitpoints_mutex_};
        hitpoints_.push_back(
            {hitpoint->position,
             hitpoint->normal,
             view_ray_direction,
             {static_cast<uint32_t>(x), static_cast<uint32_t>(y)},
             hitpoint->material_index});
      }
    }
  });
}

void CPUBackend::EvaluateRadiance(Viewport* viewport) {
  RAINBOW_TIME_FUNCTION();

  for (const auto& hitpoint : hitpoints_) {
    const auto color = viewport->GetPixel(hitpoint.pixel_location[0],
                                          hitpoint.pixel_location[1]);
    const auto& material = scene_->GetMaterial(hitpoint.material_index);
    viewport->SetPixel(
        hitpoint.pixel_location[0], hitpoint.pixel_location[1],
        color + material.diffuse_color + material.emissive_color);
  }
}

}  // namespace rainbow
