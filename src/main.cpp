#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include "camera.hpp"
#include "scene.hpp"

int main(int argc, char* argv[]) {
  SDL_Window* window;
  SDL_Renderer* renderer;

  glm::uvec2 resolution{100, 100};
  if (SDL_CreateWindowAndRenderer(resolution.x, resolution.y, 0, &window,
                                  &renderer) < 0) {
    std::cerr << "Failed to create window!" << std::endl;
    return -1;
  }

  rainbow::Scene scene(argv[1]);

  if (!scene.IsValid()) {
    std::cerr << "Failed to load `" << argv[1] << "`" << std::endl;
    return -1;
  }

  rainbow::Camera camera;
  const auto view_rays = camera.ComputeViewRays(resolution);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  const auto t0 = std::chrono::steady_clock::now();
  int x = 0;
  int y = resolution.y - 1;
  for (const auto& view_ray : view_rays) {
    const auto hitpoint = scene.ShootRay(view_ray);
    if (hitpoint) {
      aiColor3D color;
      hitpoint->material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
      SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255,
                             color.b * 255, 255);
      SDL_RenderDrawPoint(renderer, x, y);
    }
    ++x;
    if (x == resolution.x) {
      x = 0;
      --y;
    }
  }
  SDL_GL_SwapWindow(window);
  const auto t1 = std::chrono::steady_clock::now();
  std::cout
      << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
      << "ms" << std::endl;

  SDL_Event event;
  while (SDL_WaitEvent(&event)) {
    if (event.type == SDL_QUIT) {
      break;
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}
