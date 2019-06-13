#pragma once

#include "rainbow/backends/opengl/opengl.hpp"

namespace rainbow {

class Texture {
 public:
  Texture(const Texture&) = delete;
  virtual ~Texture() { glDeleteTextures(1, &gl_texture_); }
  Texture& operator=(const Texture&) = delete;

  void Bind(GLenum texture_unit) const;
  void BindImage(GLuint image_unit, GLint level, GLboolean layered, GLint layer,
                 GLenum access, GLenum format) const;

 protected:
  inline Texture(GLenum type) : type_{type} { glGenTextures(1, &gl_texture_); }

 private:
  GLuint gl_texture_;
  GLenum type_;
};

struct Texture2DDescription {
  GLsizei width = 1;
  GLsizei height = 1;
  GLsizei levels = 1;
  GLenum internal_format = GL_RGBA8;
  GLenum wrap_s = GL_CLAMP_TO_EDGE;
  GLenum wrap_t = GL_CLAMP_TO_EDGE;
  GLenum min_filter = GL_NEAREST;
  GLenum mag_filter = GL_NEAREST;
};

class Texture2D final : public Texture {
 public:
  Texture2D(const Texture2DDescription& description);
};

}  // namespace rainbow
