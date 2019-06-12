#pragma once

#include <chrono>

#include <SDL2/SDL.h>
#include "rainbow/camera.hpp"
#include "rainbow/rendering_backend.hpp"
#include "rainbow/scene.hpp"
#include "rainbow/viewport.hpp"

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

  Viewport viewport_;
  std::unique_ptr<RenderingBackend> rendering_backend_;

  void ProcessEvent(const SDL_Event& event);
  void RenderPreview();

  void EnterInteractiveMode();
  void Update(std::chrono::duration<float> elapsed_time);
  void LeaveInteractiveMode();
};

}  // namespace rainbow
