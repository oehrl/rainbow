#pragma once

#include <vector>
#include "rainbow/data_structures.hpp"
#include "rainbow/rendering_backend.hpp"

namespace rainbow {

class CPUBackend final : public RenderingBackend {
 public:
  void Prepare(const Scene& scene, size_t viewport_width,
               size_t viewport_height) override;
  void Render(const Camera& camera, Viewport* viewport) override;

 private:
  const Scene* scene_;
  std::vector<Photon> photon_buffer_;
};

}  // namespace rainbow