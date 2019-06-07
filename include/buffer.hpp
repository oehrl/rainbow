#pragma once

#include "opengl.hpp"

namespace rainbow {

class Buffer {
 public:
  explicit Buffer(GLenum type);
  Buffer(const Buffer&) = delete;
  virtual ~Buffer();

  Buffer& operator=(const Buffer&) = delete;

 private:
  GLuint gl_buffer_;
  GLenum type_;
};

}  // namespace rainbow
