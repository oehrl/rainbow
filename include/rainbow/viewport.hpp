#pragma once

#include <cstdlib>
#include <glm/vec4.hpp>
#include <vector>

namespace rainbow {

class Viewport {
 public:
  Viewport(size_t width, size_t height,
           glm::vec4 initial_color = glm::vec4{0.0});

  inline size_t GetWidth() const { return width_; }
  inline size_t GetHeight() const { return height_; }
  inline glm::vec4 GetPixel(size_t x, size_t y) const {
    return pixel_buffer_[y * width_ + x];
  }
  inline void SetPixel(size_t x, size_t y, const glm::vec4& color) {
    pixel_buffer_[y * width_ + x] = color;
  }

  void Clear(const glm::vec4& color);

 private:
  size_t width_;
  size_t height_;
  std::vector<glm::vec4> pixel_buffer_;
};

}  // namespace rainbow
