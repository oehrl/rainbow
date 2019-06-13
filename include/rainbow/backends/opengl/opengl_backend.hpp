#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include "rainbow/backends/opengl/buffer.hpp"
#include "rainbow/backends/opengl/opengl.hpp"
#include "rainbow/backends/opengl/program.hpp"
#include "rainbow/backends/opengl/texture.hpp"
#include "rainbow/rendering_backend.hpp"

namespace rainbow {

class OpenGLBackend final : public RenderingBackend {
 public:
  explicit OpenGLBackend(SDL_Window* window);
  ~OpenGLBackend() override;
  void Prepare(const Scene& scene, size_t viewport_width,
               size_t viewport_height) override;
  void Render(const Camera& camera, Viewport* viewport) override;

 private:
  SDL_Window* window_;
  SDL_GLContext opengl_context_;
  std::unique_ptr<Program> view_ray_tracing_program_;
  std::unique_ptr<Texture2D> output_texture_;

  std::unique_ptr<ShaderStorageBuffer> material_buffer_;
  std::unique_ptr<ShaderStorageBuffer> vertex_buffer_;
  std::unique_ptr<ShaderStorageBuffer> triangle_buffer_;
  uint32_t triangle_count_;
};

}  // namespace rainbow
