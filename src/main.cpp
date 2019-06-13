#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include "rainbow/application.hpp"
#include "rainbow/camera.hpp"
#include "rainbow/scene.hpp"

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO);

  rainbow::Application app;

  if (argc >= 2) {
    if (!app.LoadScene(argv[1])) {
      return EXIT_FAILURE;
    }
  }

  app.Run();

  SDL_Quit();
}
