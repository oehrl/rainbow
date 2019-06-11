#pragma once

#include "rainbow/backends/opengl/opengl.hpp"

namespace rainbow {

class Buffer {
 public:
  explicit Buffer(GLenum type);
  Buffer(const Buffer&) = delete;
  virtual ~Buffer();

  Buffer& operator=(const Buffer&) = delete;

  void Bind() { glBindBuffer(type_, gl_buffer_); }
  void BindToIndex(GLuint index) { glBindBufferBase(type_, index, gl_buffer_); }

 private:
  GLuint gl_buffer_;
  GLenum type_;
};

struct ShaderStorageBufferDescription {
  size_t size;
};

class ShaderStorageBuffer : public Buffer {
 public:
  ShaderStorageBuffer(const ShaderStorageBufferDescription& description,
                      const void* data = nullptr)
      : Buffer(GL_SHADER_STORAGE_BUFFER) {
    Bind();
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 static_cast<GLsizeiptr>(description.size), data,
                 GL_STATIC_DRAW);
  }
}

;

}  // namespace rainbow
