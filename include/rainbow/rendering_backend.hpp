#pragma once

namespace rainbow {

class Scene;
class Camera;

class RenderingBackend {
 public:
  virtual ~RenderingBackend() = default;

  virtual void Prepare(const Scene& scene) = 0;
  virtual void Render(const Camera& camera) = 0;
};

}  // namespace rainbow
