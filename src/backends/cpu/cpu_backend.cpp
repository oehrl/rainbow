#include "rainbow/backends/cpu/cpu_backend.hpp"
#include <glm/vec3.hpp>
#include "rainbow/camera.hpp"
#include "rainbow/scene.hpp"
#include "rainbow/viewport.hpp"

namespace rainbow {

void CPUBackend::Prepare(const Scene& scene) { scene_ = &scene; }

void CPUBackend::Render(const Camera& camera, Viewport* viewport) {
  glm::vec3 right;
  glm::vec3 up;
  glm::vec3 forward;
  camera.GetAxisVectors(&right, &up, &forward);

  const size_t viewport_width = viewport->GetWidth();
  const size_t viewport_height = viewport->GetHeight();

  for (size_t y = 0; y < viewport_height; ++y) {
    const float y_normalized =
        static_cast<float>(y) / (viewport_height - 1) - 0.5f;
    for (size_t x = 0; x < viewport_width; ++x) {
      const float x_normalized =
          static_cast<float>(x) / (viewport_width - 1) - 0.5f;

      const auto view_ray_direction =
          glm::normalize(x_normalized * right + y_normalized * up + forward);

      const auto hitpoint =
          scene_->ShootRay({camera.GetPosition(), view_ray_direction});

      if (hitpoint) {
        viewport->SetPixel(x, y, hitpoint->material->diffuse);
      }
    }
  }
}

}  // namespace rainbow
