#include "buffer.hpp"

namespace rainbow {

Buffer::Buffer(GLenum type) : type_(type) { glGenBuffers(1, &gl_buffer_); }
Buffer::~Buffer() { glDeleteBuffers(1, &gl_buffer_); }

}  // namespace rainbow
