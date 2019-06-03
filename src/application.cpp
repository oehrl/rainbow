#include "application.hpp"

#include <chrono>
#include <iostream>

namespace rainbow {

Application::Application() {
  if (SDL_CreateWindowAndRenderer(512, 512, 0, &window_, &renderer_) < 0) {
    std::cerr << "Failed to create window!" << std::endl;
    throw std::runtime_error("Failed to create SDL window");
  }
}

Application::~Application() {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

bool Application::LoadScene(const std::string& filename) {
  if (scene_.Load(filename)) {
    RenderPreview();
    return true;
  } else {
    std::cerr << "Failed to load `" << filename << "`" << std::endl;
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
        interactive_mode_ = true;
      }
      break;

    case SDL_MOUSEBUTTONUP:
      if (event.button.button == SDL_BUTTON_LEFT) {
        interactive_mode_ = false;
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
  int window_width;
  int window_height;
  SDL_GetWindowSize(window_, &window_width, &window_height);

  const auto view_rays =
      camera_.ComputeViewRays(glm::uvec2{window_width, window_height});
  -SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
  SDL_RenderClear(renderer_);

  const auto t0 = std::chrono::steady_clock::now();
  int x = 0;
  int y = window_height - 1;
  for (const auto& view_ray : view_rays) {
    const auto hitpoint = scene_.ShootRay(view_ray);
    if (hitpoint) {
      aiColor3D color;
      hitpoint->material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
      SDL_SetRenderDrawColor(renderer_, color.r * 255, color.g * 255,
                             color.b * 255, 255);
      SDL_RenderDrawPoint(renderer_, x, y);
    }
    ++x;
    if (x == window_width) {
      x = 0;
      --y;
    }
  }
  SDL_GL_SwapWindow(window_);

  const auto t1 = std::chrono::steady_clock::now();
  std::cout
      << "RenderPreview(): "
      << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
      << "ms" << std::endl;
}

}  // namespace rainbow
