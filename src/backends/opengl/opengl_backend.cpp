#include "rainbow/backends/opengl/opengl_backend.hpp"
#include <glm/vec3.hpp>
#include <iostream>
#include "rainbow/camera.hpp"
#include "rainbow/scene.hpp"
#include "rainbow/timing.hpp"
#include "rainbow/viewport.hpp"
#include "shaders.hpp"

namespace rainbow {

OpenGLBackend::OpenGLBackend(SDL_Window* window) : window_(window) {
  GLint gl_major_version;
  glGetIntegerv(GL_MAJOR_VERSION, &gl_major_version);
  GLint gl_minor_version;
  glGetIntegerv(GL_MINOR_VERSION, &gl_minor_version);
  std::cout << "Using OpenGL " << gl_major_version << "." << gl_minor_version
            << std::endl;

  view_ray_tracing_program_ = std::make_unique<Program>();
  view_ray_tracing_program_->AttachShader(GL_COMPUTE_SHADER,
                                          shaders::test_comp);
  view_ray_tracing_program_->Link();
}

void OpenGLBackend::Prepare(const Scene& scene, size_t viewport_width,
                            size_t viewport_height) {
  RAINBOW_TIME_FUNCTION();

  Texture2DDescription output_texture_description;
  output_texture_description.width = viewport_width;
  output_texture_description.height = viewport_height;
  output_texture_description.internal_format = GL_RGBA32F;
  output_texture_ = std::make_unique<Texture2D>(output_texture_description);

  ShaderStorageBufferDescription desc;

  const auto& materials = scene.GetMaterials();
  desc.size = materials.size() * sizeof(Scene::Material);
  material_buffer_ =
      std::make_unique<ShaderStorageBuffer>(desc, materials.data());

  const auto& vertices = scene.GetVertices();
  desc.size = vertices.size() * sizeof(Scene::Vertex);
  vertex_buffer_ = std::make_unique<ShaderStorageBuffer>(desc, vertices.data());

  const auto& triangles = scene.GetTriangles();
  desc.size = triangles.size() * sizeof(Scene::Triangle);
  triangle_buffer_ =
      std::make_unique<ShaderStorageBuffer>(desc, triangles.data());

  triangle_count_ = scene.GetTriangleCount();
}

void OpenGLBackend::Render(const Camera& camera, Viewport* viewport) {
  RAINBOW_TIME_FUNCTION();

  glm::vec3 right;
  glm::vec3 up;
  glm::vec3 forward;
  camera.GetAxisVectors(&right, &up, &forward);
  glm::vec3 camera_position = camera.GetPosition();

  view_ray_tracing_program_->Use();
  output_texture_->BindImage(0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  material_buffer_->BindToIndex(0);
  vertex_buffer_->BindToIndex(1);
  triangle_buffer_->BindToIndex(2);
  glUniform3fv(view_ray_tracing_program_->GetUniformLocation("u_Right"), 1,
               &right.x);
  glUniform3fv(view_ray_tracing_program_->GetUniformLocation("u_Up"), 1, &up.x);
  glUniform3fv(view_ray_tracing_program_->GetUniformLocation("u_Forward"), 1,
               &forward.x);
  glUniform3fv(
      view_ray_tracing_program_->GetUniformLocation("u_CameraPosition"), 1,
      &camera_position.x);
  glUniform1ui(view_ray_tracing_program_->GetUniformLocation("u_TriangleCount"),
               triangle_count_);
  glDispatchCompute(viewport->GetWidth(), viewport->GetHeight(), 1);

  // Wait until writes to the images are finished
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  output_texture_->Bind(GL_TEXTURE0);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, viewport->GetPixels());
}

}  // namespace rainbow
