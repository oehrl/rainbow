#pragma once

#include <vulkan/vulkan.hpp>

namespace rainbow {

void CheckVulkanResult(VkResult result, const char* filename = nullptr,
                       int line = 0);

}  // namespace rainbow

#define RAINBOW_CHECK_VK_RESULT(result) \
  CheckVulkanResult((result), __FILE__, __LINE__)
