#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include "scene.hpp"

int main(int argc, char* argv[]) {
  SDL_Window* window;
  SDL_Renderer* renderer;
  if (SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer) < 0) {
    std::cerr << "Failed to create window!" << std::endl;
    return -1;
  }

  rainbow::Scene scene(argv[1]);

  const auto t0 = std::chrono::steady_clock::now();
  for (int i = 0; i < 10; ++i) {
    scene.ShootRay({{}, {0.0f, 0.0f, 1.0f}});
  }
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
