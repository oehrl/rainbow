#pragma once

#include <memory>
#include "rainbow/rendering_backend.hpp"

namespace rainbow {

std::unique_ptr<RenderingBackend> MakeMetalBackend();

}  // namespace rainbow