#include "metal_backend.hh"
#include "rainbow/shaders/library.metal.hpp"
#include "rainbow/timing.hpp"
#include "rainbow/viewport.hpp"
#include "rainbow/scene.hpp"
#include "rainbow/camera.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace rainbow {

std::unique_ptr<RenderingBackend> MakeMetalBackend() {
  return std::make_unique<MetalBackend>();
}

void ThrowOnError(NSError *error) {
  if (error) {
    throw std::runtime_error(
        [[error description] cStringUsingEncoding:kCFStringEncodingUTF8]);
  }
}

MetalBackend::MetalBackend() : device_{MTLCreateSystemDefaultDevice()} {
  NSError *error;
  if (!device_) {
    throw std::runtime_error("Failed to create Metal device");
  }

  library_ = [device_
      newLibraryWithSource:[NSString
                               stringWithUTF8String:shaders::library_metal]
                   options:nil
                     error:&error];
     if (library_ == nullptr) {
         ThrowOnError(error);
     }
  assert(library_);

  view_ray_function_ = [library_ newFunctionWithName:@"ShootViewRays"];
  assert(view_ray_function_);

  compute_pipeline_state_ =
      [device_ newComputePipelineStateWithFunction:view_ray_function_
                                             error:&error];
  ThrowOnError(error);

  command_queue_ = [device_ newCommandQueue];
  assert(command_queue_);
}

void MetalBackend::Prepare(const Scene &scene, size_t viewport_width,
                           size_t viewport_height) {

  MTLTextureDescriptor *texture_descriptor =
      [[MTLTextureDescriptor alloc] init];

  texture_descriptor.textureType = MTLTextureType2D;
  texture_descriptor.pixelFormat = MTLPixelFormatRGBA32Float;
  texture_descriptor.width = viewport_width;
  texture_descriptor.height = viewport_height;
  texture_descriptor.usage = MTLTextureUsageShaderWrite;

  output_texture_ = [device_ newTextureWithDescriptor:texture_descriptor];
  if (!output_texture_) {
    throw std::runtime_error("Failed to create output texture!");
  }

  [texture_descriptor release];
    
    
    parameters_buffer_ = [device_ newBufferWithLength:sizeof(parameters_) options:MTLResourceCPUCacheModeWriteCombined];
    
    const auto& materials = scene.GetMaterials();
    material_buffer_ = [device_ newBufferWithBytes:materials.data() length:materials.size() * sizeof(Scene::Material) options:MTLResourceCPUCacheModeWriteCombined];
    
    
    const auto& vertices = scene.GetVertices();
    vertex_buffer_ = [device_ newBufferWithBytes:vertices.data() length:vertices.size() * sizeof(Scene::Vertex) options:MTLResourceCPUCacheModeWriteCombined];
    
    const auto& triangles = scene.GetTriangles();
    triangle_buffer_ = [device_ newBufferWithBytes:triangles.data() length:triangles.size() * sizeof(Scene::Triangle) options:MTLResourceCPUCacheModeWriteCombined];
    
    parameters_.triangle_count = triangles.size();
    parameters_.resolution = simd_make_float2(viewport_width, viewport_height);
}

void MetalBackend::Render(const Camera &camera, Viewport *viewport) {
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 forward;
    camera.GetAxisVectors(&right, &up, &forward);
    
   const glm::vec3 camera_position = camera.GetPosition();
    
    parameters_.right = simd_make_float3(right.x, right.y, right.z);
    parameters_.up = simd_make_float3(up.x, up.y, up.z);
    parameters_.forward = simd_make_float3(forward.x, forward.y, forward.z);
    parameters_.camera_position = simd_make_float3(camera_position.x, camera_position.y, camera_position.z);
    memcpy([parameters_buffer_ contents], &parameters_, sizeof(parameters_));
    
  id<MTLCommandBuffer> command_buffer = [command_queue_ commandBuffer];
  assert(command_buffer);

  id<MTLComputeCommandEncoder> compute_encoder =
      [command_buffer computeCommandEncoder];
  assert(compute_encoder);

  [compute_encoder setComputePipelineState:compute_pipeline_state_];

  [compute_encoder setTexture:output_texture_ atIndex:0];
    [compute_encoder setBuffer:parameters_buffer_ offset:0 atIndex:0];
    [compute_encoder setBuffer:material_buffer_ offset:0 atIndex:1];
    [compute_encoder setBuffer:vertex_buffer_ offset:0 atIndex:2];
    [compute_encoder setBuffer:triangle_buffer_ offset:0 atIndex:3];

  MTLSize thread_group_size = MTLSizeMake(16, 16, 1);
  MTLSize thread_group_count;
  thread_group_count.width =
      (viewport->GetWidth() + thread_group_size.width - 1) /
      thread_group_size.width;
  thread_group_count.height =
      (viewport->GetHeight() + thread_group_size.height - 1) /
      thread_group_size.height;
  thread_group_count.depth = 1;
  [compute_encoder dispatchThreadgroups:thread_group_count
                  threadsPerThreadgroup:thread_group_size];
  [compute_encoder endEncoding];

  id<MTLBlitCommandEncoder> blit_encoder = [command_buffer blitCommandEncoder];
  [blit_encoder synchronizeTexture:output_texture_ slice:0 level:0];
  [blit_encoder endEncoding];

  [command_buffer commit];

  [command_buffer waitUntilCompleted];
  [output_texture_ getBytes:viewport->GetPixels()
                bytesPerRow:viewport->GetWidth() * sizeof(glm::vec4)
                 fromRegion:MTLRegionMake2D(0, 0, viewport->GetWidth(),
                                            viewport->GetHeight())
                mipmapLevel:0];
}

}
