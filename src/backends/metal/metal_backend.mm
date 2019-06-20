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

  generate_hitpoints_function_ = [library_ newFunctionWithName:@"GenerateHitpoints"];
  assert(generate_hitpoints_function_);
    
    generate_hitpoints_pipeline_state_ =
    [device_ newComputePipelineStateWithFunction:generate_hitpoints_function_
                                           error:&error];
    ThrowOnError(error);
    
    evaluate_radiance_function_ = [library_ newFunctionWithName:@"EvaluateRadiance"];
    assert(evaluate_radiance_function_);
    
    evaluate_radiance_pipeline_state_ =
    [device_ newComputePipelineStateWithFunction:evaluate_radiance_function_
                                           error:&error];
    ThrowOnError(error);
    
    clear_texture_function_ = [library_ newFunctionWithName:@"ClearTexture"];
    assert(clear_texture_function_);
    
    clear_texture_pipeline_state_ =
    [device_ newComputePipelineStateWithFunction:clear_texture_function_
                                           error:&error];
    ThrowOnError(error);

  command_queue_ = [device_ newCommandQueue];
  assert(command_queue_);
    
    parameters_buffer_ = [device_ newBufferWithLength:sizeof(parameters_) options:MTLResourceCPUCacheModeWriteCombined];
    
    hitpoint_counter_buffer_ = [device_ newBufferWithLength:4 options:MTLResourceCPUCacheModeWriteCombined];
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
    
    
    const auto& materials = scene.GetMaterials();
    material_buffer_ = [device_ newBufferWithBytes:materials.data() length:materials.size() * sizeof(Material) options:MTLResourceCPUCacheModeWriteCombined];
    
    
    const auto& vertices = scene.GetVertexPositions();
    vertex_buffer_ = [device_ newBufferWithBytes:vertices.data() length:vertices.size() * sizeof(Vector3) options:MTLResourceCPUCacheModeWriteCombined];
    
    const auto& triangles = scene.GetTriangles();
    triangle_buffer_ = [device_ newBufferWithBytes:triangles.data() length:triangles.size() * sizeof(TriangleReference) options:MTLResourceCPUCacheModeWriteCombined];
    
    uint32 hitpoint_buffer_entries = viewport_width * viewport_height;
    uint32 hitpoint_buffer_size = hitpoint_buffer_entries * 64;
    hitpoint_buffer_ = [device_ newBufferWithLength:hitpoint_buffer_size options:MTLResourceStorageModePrivate];
    
    parameters_.triangle_count = triangles.size();
    parameters_.resolution = simd_make_float2(viewport_width, viewport_height);
    parameters_.max_hitpoint_count = hitpoint_buffer_entries;
}

void MetalBackend::Render(const Camera &camera, Viewport *viewport) {
    Vector3 right;
    Vector3 up;
    Vector3 forward;
    camera.GetAxisVectors(&right, &up, &forward);
    
   const Vector3 camera_position = camera.GetPosition();
    
    parameters_.right = simd_make_float3(right.x, right.y, right.z);
    parameters_.up = simd_make_float3(up.x, up.y, up.z);
    parameters_.forward = simd_make_float3(forward.x, forward.y, forward.z);
    parameters_.camera_position = simd_make_float3(camera_position.x, camera_position.y, camera_position.z);
    memcpy([parameters_buffer_ contents], &parameters_, sizeof(parameters_));
    
    const uint32 initial_hitpoint_counter_value = 0;
    memcpy([hitpoint_counter_buffer_ contents], &initial_hitpoint_counter_value, sizeof(initial_hitpoint_counter_value));
    
  id<MTLCommandBuffer> command_buffer = [command_queue_ commandBuffer];
  assert(command_buffer);

  id<MTLComputeCommandEncoder> compute_encoder =
      [command_buffer computeCommandEncoder];
  assert(compute_encoder);
    
    [compute_encoder setTexture:output_texture_ atIndex:0];
    [compute_encoder setBuffer:parameters_buffer_ offset:0 atIndex:0];
    [compute_encoder setBuffer:material_buffer_ offset:0 atIndex:1];
    [compute_encoder setBuffer:vertex_buffer_ offset:0 atIndex:2];
    [compute_encoder setBuffer:triangle_buffer_ offset:0 atIndex:3];
    [compute_encoder setBuffer:hitpoint_counter_buffer_ offset:0 atIndex:4];
    [compute_encoder setBuffer:hitpoint_buffer_ offset:0 atIndex:5];

    {
        [compute_encoder setComputePipelineState:generate_hitpoints_pipeline_state_];
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
    }
    {
        [compute_encoder setComputePipelineState:clear_texture_pipeline_state_];
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
    }
    {
        uint32 hitpoint_count;
        memcpy(&hitpoint_count, [hitpoint_counter_buffer_ contents], sizeof(hitpoint_count));
        std::cout << "Number of hitpoints: " << hitpoint_count << std::endl;
        
        
    [compute_encoder setComputePipelineState:evaluate_radiance_pipeline_state_];
        NSUInteger pixel_count = viewport->GetWidth() * viewport->GetHeight();
        MTLSize thread_group_size = MTLSizeMake(64, 1, 1);
        MTLSize thread_group_count = MTLSizeMake((pixel_count + thread_group_size.width - 1) /
                                                 thread_group_size.width, 1, 1);
        [compute_encoder dispatchThreadgroups:thread_group_count threadsPerThreadgroup:thread_group_size];
    }
  [compute_encoder endEncoding];

  id<MTLBlitCommandEncoder> blit_encoder = [command_buffer blitCommandEncoder];
  [blit_encoder synchronizeTexture:output_texture_ slice:0 level:0];
  [blit_encoder endEncoding];

  [command_buffer commit];

  [command_buffer waitUntilCompleted];
  [output_texture_ getBytes:viewport->GetPixels()
                bytesPerRow:viewport->GetWidth() * sizeof(Vector4)
                 fromRegion:MTLRegionMake2D(0, 0, viewport->GetWidth(),
                                            viewport->GetHeight())
                mipmapLevel:0];
}

}
