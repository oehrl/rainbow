#include "rainbow/backends/opengl/program.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace rainbow {

bool Program::AttachShader(GLenum shader_type, std::string_view shader_source) {
  const GLuint shader = glCreateShader(shader_type);
  assert(shader != 0);

  const GLchar* source = shader_source.data();
  const GLint source_length = shader_source.size();
  glShaderSource(shader, 1, &source, &source_length);
  glCompileShader(shader);

  GLint compile_status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
  if (compile_status != GL_TRUE) {
    GLint info_log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

    std::vector<GLchar> info_log_buffer(info_log_length);
    glGetShaderInfoLog(shader, info_log_length, nullptr,
                       info_log_buffer.data());
    std::cerr << "Failed to compile shader:\n"
              << info_log_buffer.data() << std::endl;
  } else {
    glAttachShader(gl_program_, shader);
  }

  glDeleteShader(shader);

  return compile_status == GL_TRUE;
}

bool Program::Link() {
  glLinkProgram(gl_program_);

  GLint link_status;
  glGetProgramiv(gl_program_, GL_LINK_STATUS, &link_status);
  if (link_status != GL_TRUE) {
    GLint info_log_length;
    glGetProgramiv(gl_program_, GL_INFO_LOG_LENGTH, &info_log_length);

    std::vector<GLchar> info_log_buffer(info_log_length);
    glGetProgramInfoLog(gl_program_, info_log_length, nullptr,
                        info_log_buffer.data());
    std::cerr << "Failed to link program:\n"
              << info_log_buffer.data() << std::endl;
  }

  return link_status == GL_TRUE;
}

bool Program::IsLinkedSucessfully() {
  GLint link_status;
  glGetProgramiv(gl_program_, GL_LINK_STATUS, &link_status);
  return link_status == true;
}

void Program::Use() {
  assert(IsLinkedSucessfully());
  glUseProgram(gl_program_);
}

GLint Program::GetUniformLocation(const char* uniform_name) {
  return glGetUniformLocation(gl_program_, uniform_name);
}

}  // namespace rainbow
