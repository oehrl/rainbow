#include "rainbow/backends/opengl/texture.hpp"
#include <cassert>

namespace rainbow {

void Texture::Bind(GLenum texture_unit) const {
  glActiveTexture(texture_unit);
  glBindTexture(type_, gl_texture_);
}

void Texture::BindImage(GLuint image_unit, GLint level, GLboolean layered,
                        GLint layer, GLenum access, GLenum format) const {
  glBindImageTexture(image_unit, gl_texture_, level, layered, layer, access,
                     format);
}

Texture2D::Texture2D(const Texture2DDescription& description)
    : Texture{GL_TEXTURE_2D} {
  glGetError();
  Bind(GL_TEXTURE0);
  glTexStorage2D(GL_TEXTURE_2D, description.levels, description.internal_format,
                 description.width, description.height);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, description.min_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, description.mag_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, description.wrap_s);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, description.wrap_t);
}

}  // namespace rainbow
