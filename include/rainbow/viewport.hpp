#pragma once

#include <cstdlib>
#include <vector>
#include "rainbow/vector.hpp"

namespace rainbow {

class Viewport {
 public:
  Viewport(size_t width, size_t height,
           Vector4 initial_color = Vector4::Zero());

  inline size_t GetWidth() const { return width_; }
  inline size_t GetHeight() const { return height_; }
  inline Vector4 GetPixel(size_t x, size_t y) const {
    return pixel_buffer_[y * width_ + x];
  }
  inline void SetPixel(size_t x, size_t y, const Vector4& color) {
    pixel_buffer_[y * width_ + x] = color;
  }

  inline const Vector4* GetPixels() const { return pixel_buffer_.data(); }
  inline Vector4* GetPixels() { return pixel_buffer_.data(); }

  void Clear(const Vector4& color);

 private:
  size_t width_;
  size_t height_;
  std::vector<Vector4> pixel_buffer_;
};

}  // namespace rainbow
