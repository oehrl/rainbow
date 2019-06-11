#pragma once

#include <cassert>
#include <string_view>
#include "rainbow/backends/opengl/opengl.hpp"

namespace rainbow {

class Program final {
 public:
  inline Program() : gl_program_{glCreateProgram()} {
    assert(gl_program_ != 0);
  }
  Program(const Program&) = delete;
  ~Program() { glDeleteProgram(gl_program_); }

  Program& operator=(const Program&) = delete;

  bool AttachShader(GLenum shader_type, std::string_view shader_source);
  bool Link();
  bool IsLinkedSucessfully();
  void Use();
  GLint GetUniformLocation(const char* uniform_name);

 private:
  GLuint gl_program_;
};

}  // namespace rainbow