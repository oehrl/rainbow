#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace rainbow {
namespace vulkan {

std::vector<VkPhysicalDevice> GetPhysicalDevices(VkInstance instance);
std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice device);
std::vector<uint32_t> GetQueueFamilyIndicesForFlags(VkPhysicalDevice device,
                                                    VkQueueFlags flags);

}  // namespace vulkan
}  // namespace rainbow
