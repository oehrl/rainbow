#include "application.hpp"

#include <chrono>
#include <iostream>

#include <glm/geometric.hpp>

#include "shaders.hpp"
#include "timing.hpp"

namespace rainbow {

Application::Application() {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  if (SDL_CreateWindowAndRenderer(512, 512, SDL_WINDOW_OPENGL, &window_,
                                  &renderer_) < 0) {
    std::cerr << "Failed to create window!" << std::endl;
    throw std::runtime_error("Failed to create SDL window");
  }
  opengl_context_ = SDL_GL_CreateContext(window_);

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

  fullscreen_quad_program_ = std::make_unique<Program>();
  fullscreen_quad_program_->AttachShader(GL_VERTEX_SHADER,
                                         shaders::fullscreen_quad_vert);
  fullscreen_quad_program_->AttachShader(GL_FRAGMENT_SHADER,
                                         shaders::fullscreen_quad_frag);
  fullscreen_quad_program_->Link();

  Texture2DDescription output_texture_description;
  output_texture_description.width = 512;
  output_texture_description.height = 512;
  output_texture_description.internal_format = GL_RGBA32F;
  output_texture_ = std::make_unique<Texture2D>(output_texture_description);

  glGenVertexArrays(1, &vao_);
}

Application::~Application() {
  SDL_GL_DeleteContext(opengl_context_);
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

bool Application::LoadScene(const std::string& filename) {
  if (scene_.Load(filename)) {
    RenderPreview();
    return true;
  } else {
    std::string error_string = "Failed to load scene: `" + filename + "`";
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Loading failed",
                             error_string.c_str(), window_);
    std::cerr << error_string << std::endl;
    return false;
  }
}

void Application::Run() {
  SDL_Event event;
  while (!quit_application_) {
    if (interactive_mode_) {
      while (SDL_PollEvent(&event)) {
        ProcessEvent(event);
      }
      const auto current_time = std::chrono::steady_clock::now();
      Update(current_time - last_update_);
      last_update_ = current_time;

    } else {
      if (SDL_WaitEvent(&event)) {
        ProcessEvent(event);
      }
    }

    if (redraw_preview_) {
      RenderPreview();
      redraw_preview_ = false;
    }
  }
}

void Application::ProcessEvent(const SDL_Event& event) {
  switch (event.type) {
    case SDL_QUIT:
      quit_application_ = true;
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (event.button.button == SDL_BUTTON_LEFT) {
        EnterInteractiveMode();
      }
      break;

    case SDL_MOUSEBUTTONUP:
      if (event.button.button == SDL_BUTTON_LEFT) {
        LeaveInteractiveMode();
      }
      break;

    case SDL_MOUSEMOTION:
      if (interactive_mode_) {
        camera_.Rotate(event.motion.xrel * 0.01f, event.motion.yrel * 0.01f);
        redraw_preview_ = true;
      }
      break;
  }
}

void Application::RenderPreview() {
  RAINBOW_TIME_FUNCTION();

  int window_width;
  int window_height;
  SDL_GetWindowSize(window_, &window_width, &window_height);

  // RAINBOW_TIME_SECTION("Compute view rays") {
  //   camera_.ComputeViewDirections(glm::uvec2{window_width, window_height},
  //                                 &view_direction_buffer_);
  // };

  glm::vec3 right;
  glm::vec3 up;
  glm::vec3 forward;
  camera_.GetAxisVectors(&right, &up, &forward);
  glm::vec3 camera_position = camera_.GetPosition();

  view_ray_tracing_program_->Use();
  output_texture_->BindImage(0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  scene_.GetMaterialBuffer()->BindToIndex(0);
  scene_.GetVertexBuffer()->BindToIndex(1);
  scene_.GetTriangleBuffer()->BindToIndex(2);
  glUniform3fv(view_ray_tracing_program_->GetUniformLocation("u_Right"), 1,
               &right.x);
  glUniform3fv(view_ray_tracing_program_->GetUniformLocation("u_Up"), 1, &up.x);
  glUniform3fv(view_ray_tracing_program_->GetUniformLocation("u_Forward"), 1,
               &forward.x);
  glUniform3fv(
      view_ray_tracing_program_->GetUniformLocation("u_CameraPosition"), 1,
      &camera_position.x);
  glUniform1ui(view_ray_tracing_program_->GetUniformLocation("u_TriangleCount"),
               scene_.GetTriangleCount());
  glDispatchCompute(512, 512, 1);

  // Wait until writes to the images are finished
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  fullscreen_quad_program_->Use();
  glBindVertexArray(vao_);
  output_texture_->Bind(GL_TEXTURE0);
  glUniform1i(fullscreen_quad_program_->GetUniformLocation("u_TextureSampler"),
              0);
  glUniform2f(fullscreen_quad_program_->GetUniformLocation("u_ViewportSize"),
              512.0f, 512.0f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  SDL_GL_SwapWindow(window_);
}

void Application::EnterInteractiveMode() {
  interactive_mode_ = true;
  last_update_ = std::chrono::steady_clock::now();
}

void Application::Update(std::chrono::duration<float> elapsed_time) {
  glm::vec3 move_vector{0.0f, 0.0f, 0.0f};
  const Uint8* keys = SDL_GetKeyboardState(nullptr);

  glm::vec3 right;
  glm::vec3 up;
  glm::vec3 forward;
  camera_.GetAxisVectors(&right, &up, &forward);

  if (keys[SDL_SCANCODE_W]) {
    move_vector += forward;
  }
  if (keys[SDL_SCANCODE_S]) {
    move_vector -= forward;
  }
  if (keys[SDL_SCANCODE_D]) {
    move_vector += right;
  }
  if (keys[SDL_SCANCODE_A]) {
    move_vector -= right;
  }
  if (keys[SDL_SCANCODE_E]) {
    move_vector += up;
  }
  if (keys[SDL_SCANCODE_Q]) {
    move_vector -= up;
  }

  float length = glm::length(move_vector);
  if (length > std::numeric_limits<float>::epsilon()) {
    camera_.Move(move_vector * (elapsed_time.count() / length));
    redraw_preview_ = true;
  }
}

void Application::LeaveInteractiveMode() { interactive_mode_ = false; }

}  // namespace rainbow
