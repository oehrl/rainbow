#include "rainbow/backends/vulkan/vulkan.hpp"
#include "rainbow/backends/vulkan/error.hpp"

namespace rainbow {
namespace vulkan {

std::vector<VkPhysicalDevice> GetPhysicalDevices(VkInstance instance) {
  uint32_t physical_device_count;
  RAINBOW_CHECK_VK_RESULT(
      vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr));
  std::vector<VkPhysicalDevice> physical_devices{physical_device_count};
  RAINBOW_CHECK_VK_RESULT(vkEnumeratePhysicalDevices(
      instance, &physical_device_count, physical_devices.data()));

  return physical_devices;
}

std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice device) {
  uint32_t queue_family_properties_count;
  vkGetPhysicalDeviceQueueFamilyProperties(
      device, &queue_family_properties_count, nullptr);
  std::vector<VkQueueFamilyProperties> queue_family_properties{
      queue_family_properties_count};
  vkGetPhysicalDeviceQueueFamilyProperties(
      device, &queue_family_properties_count, queue_family_properties.data());
  return queue_family_properties;
}

std::vector<uint32_t> GetQueueFamilyIndicesForFlags(VkPhysicalDevice device,
                                                    VkQueueFlags flags) {
  std::vector<uint32_t> suitable_queue_family_indices;
  uint32_t queue_family_index = 0;
  for (const auto& queue_family_properties :
       GetPhysicalDeviceQueueFamilyProperties(device)) {
    if ((queue_family_properties.queueFlags & flags) == flags) {
      suitable_queue_family_indices.push_back(queue_family_index);
    }
    ++queue_family_index;
  }
  return suitable_queue_family_indices;
}

}  // namespace vulkan
}  // namespace rainbow
