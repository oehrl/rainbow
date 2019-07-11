#include "rainbow/backends/vulkan/error.hpp"

#include <iostream>

namespace rainbow {

void CheckVulkanResult(VkResult result, const char* filename, int line) {
  const char* error_name;
  switch (result) {
    case VK_SUCCESS:
      return;

    case VK_NOT_READY:
      error_name = "VK_NOT_READY";
      break;
    case VK_TIMEOUT:
      error_name = "VK_TIMEOUT";
      break;
    case VK_EVENT_SET:
      error_name = "VK_EVENT_SET";
      break;
    case VK_EVENT_RESET:
      error_name = "VK_EVENT_RESET";
      break;
    case VK_INCOMPLETE:
      error_name = "VK_INCOMPLETE";
      break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      error_name = "VK_ERROR_OUT_OF_HOST_MEMORY";
      break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      error_name = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
      break;
    case VK_ERROR_INITIALIZATION_FAILED:
      error_name = "VK_ERROR_INITIALIZATION_FAILED";
      break;
    case VK_ERROR_DEVICE_LOST:
      error_name = "VK_ERROR_DEVICE_LOST";
      break;
    case VK_ERROR_MEMORY_MAP_FAILED:
      error_name = "VK_ERROR_MEMORY_MAP_FAILED";
      break;
    case VK_ERROR_LAYER_NOT_PRESENT:
      error_name = "VK_ERROR_LAYER_NOT_PRESENT";
      break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      error_name = "VK_ERROR_EXTENSION_NOT_PRESENT";
      break;
    case VK_ERROR_FEATURE_NOT_PRESENT:
      error_name = "VK_ERROR_FEATURE_NOT_PRESENT";
      break;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      error_name = "VK_ERROR_INCOMPATIBLE_DRIVER";
      break;
    case VK_ERROR_TOO_MANY_OBJECTS:
      error_name = "VK_ERROR_TOO_MANY_OBJECTS";
      break;
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
      error_name = "VK_ERROR_FORMAT_NOT_SUPPORTED";
      break;
    case VK_ERROR_FRAGMENTED_POOL:
      error_name = "VK_ERROR_FRAGMENTED_POOL";
      break;
    case VK_ERROR_OUT_OF_POOL_MEMORY:
      error_name = "VK_ERROR_OUT_OF_POOL_MEMORY";
      break;
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
      error_name = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
      break;
    case VK_ERROR_SURFACE_LOST_KHR:
      error_name = "VK_ERROR_SURFACE_LOST_KHR";
      break;
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
      error_name = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
      break;
    case VK_SUBOPTIMAL_KHR:
      error_name = "VK_SUBOPTIMAL_KHR";
      break;
    case VK_ERROR_OUT_OF_DATE_KHR:
      error_name = "VK_ERROR_OUT_OF_DATE_KHR";
      break;
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
      error_name = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
      break;
    case VK_ERROR_VALIDATION_FAILED_EXT:
      error_name = "VK_ERROR_VALIDATION_FAILED_EXT";
      break;
    case VK_ERROR_INVALID_SHADER_NV:
      error_name = "VK_ERROR_INVALID_SHADER_NV";
      break;
    case VK_RESULT_RANGE_SIZE:
      error_name = "VK_RESULT_RANGE_SIZE";
      break;

    default:
      error_name = "Unknown Error";
  };

  std::cerr << "Vulkan error: " << error_name;
  if (filename) {
    std::cerr << " at " << filename << ":" << line;
  }
  std::cerr << std::endl;
}

}  // namespace rainbow
