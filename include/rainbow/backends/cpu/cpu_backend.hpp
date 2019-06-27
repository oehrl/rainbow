#pragma once

#include <mutex>
#include <vector>
#include "rainbow/data_structures.hpp"
#include "rainbow/photon_map.hpp"
#include "rainbow/rendering_backend.hpp"

namespace rainbow {

class CPUBackend final : public RenderingBackend {
 public:
  void Prepare(const Scene& scene, size_t viewport_width,
               size_t viewport_height) override;
  void Render(const Camera& camera, Viewport* viewport) override;

 private:
  const Scene* scene_;
  std::mutex hitpoints_mutex_;
  std::vector<Hitpoint> hitpoints_;
  std::vector<Photon> emitted_photons_buffer_;
  std::vector<Photon> photon_buffer_;
  PhotonMap photon_map_;
  size_t emitted_photons_count_;

  void GenerateHitpoints(const Camera& camera, size_t viewport_width,
                         size_t viewport_height);
  void EvaluateRadiance(Viewport* viewport);
};

}  // namespace rainbow