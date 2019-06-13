#include "rainbow/viewport.hpp"

namespace rainbow {

Viewport::Viewport(size_t width, size_t height, glm::vec4 initial_color)
    : width_{width},
      height_{height},
      pixel_buffer_{width * height, initial_color} {}

void Viewport::Clear(const glm::vec4& color) {
  for (auto& pixel : pixel_buffer_) {
    pixel = color;
  }
}

}  // namespace rainbow
