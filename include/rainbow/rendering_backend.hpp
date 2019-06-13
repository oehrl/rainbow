#pragma once

#include <cstdlib>

namespace rainbow {

class Scene;
class Camera;
class Viewport;

class RenderingBackend {
 public:
  virtual ~RenderingBackend() = default;

  virtual void Prepare(const Scene& scene, size_t viewport_width,
                       size_t viewport_height) = 0;
  virtual void Render(const Camera& camera, Viewport* viewport) = 0;
};

}  // namespace rainbow
