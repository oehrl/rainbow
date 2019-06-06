#pragma once

#include <chrono>

#include <SDL2/SDL.h>
#include "camera.hpp"
#include "program.hpp"
#include "scene.hpp"
#include "texture.hpp"

namespace rainbow {

class Application final {
 public:
  Application();
  Application(const Application&) = delete;
  Application(Application&&) = delete;
  ~Application();

  Application operator=(const Application&) = delete;
  Application operator=(Application&&) = delete;

  bool LoadScene(const std::string& filename);

  void Run();

 private:
  Scene scene_;
  Camera camera_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  SDL_GLContext opengl_context_;

  bool quit_application_ = false;
  bool interactive_mode_ = false;
  bool redraw_preview_ = false;

  std::chrono::steady_clock::time_point last_update_;

  std::vector<glm::vec3> view_direction_buffer_;

  std::unique_ptr<Program> view_ray_tracing_program_;
  std::unique_ptr<Program> fullscreen_quad_program_;
  std::unique_ptr<Texture2D> output_texture_;
  GLuint vao_;

  void ProcessEvent(const SDL_Event& event);
  void RenderPreview();

  void EnterInteractiveMode();
  void Update(std::chrono::duration<float> elapsed_time);
  void LeaveInteractiveMode();
};

}  // namespace rainbow
