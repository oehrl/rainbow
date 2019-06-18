#pragma once

#include "rainbow/rendering_backend.hpp"
#include <memory>

namespace rainbow {

std::unique_ptr<RenderingBackend> MakeMetalBackend();

} // namespace rainbow