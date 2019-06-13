#pragma once
#import "Metal/Metal.h"
#include "rainbow/rendering_backend.hpp"

namespace rainbow {

class MetalBackend final : public RenderingBackend {
public:
  MetalBackend();

  void Prepare(const Scene &scene, size_t viewport_width,
               size_t viewport_height) override;
  void Render(const Camera &camera, Viewport *viewport) override;

private:
  id<MTLDevice> device_;
  id<MTLLibrary> library_;
  id<MTLFunction> view_ray_function_;
  id<MTLComputePipelineState> compute_pipeline_state_;
  id<MTLTexture> output_texture_;
  id<MTLCommandQueue> command_queue_;
};

} // namespace rainbow
