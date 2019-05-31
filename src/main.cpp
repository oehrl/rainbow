#include <chrono>
#include <iostream>
#include "scene.hpp"

int main(int argc, char* argv[]) {
  rainbow::Scene scene(argv[1]);

  const auto t0 = std::chrono::steady_clock::now();
  for (int i = 0; i < 10; ++i) {
    scene.ShootRay({{}, {0.0f, 0.0f, 1.0f}});
  }
  const auto t1 = std::chrono::steady_clock::now();
  std::cout
      << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
      << "ms" << std::endl;
}
