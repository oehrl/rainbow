#pragma once

#include <vulkan/vulkan.hpp>
#include "rainbow/rendering_backend.hpp"

namespace rainbow {

class VulkanBackend final : public RenderingBackend {
 public:
  VulkanBackend();
  ~VulkanBackend() override;

  void Prepare(const Scene& scene, size_t viewport_width,
               size_t viewport_height) override;
  void Render(const Camera& camera, Viewport* viewport) override;

 private:
  VkInstance instance_;
};

}  // namespace rainbow