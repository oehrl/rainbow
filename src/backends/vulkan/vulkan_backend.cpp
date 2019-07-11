#include "rainbow/backends/vulkan/vulkan_backend.hpp"
#include <iostream>
#include <rainbow/backends/vulkan/error.hpp>

namespace rainbow {
VulkanBackend::VulkanBackend() {
  VkApplicationInfo app_info;
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = nullptr;
  app_info.pApplicationName = "rainbow";
  app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
  app_info.pEngineName = "rainbow";
  app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
  app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);

  VkInstanceCreateInfo instance_create_info;
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pNext = nullptr;
  instance_create_info.flags = 0;
  instance_create_info.pApplicationInfo = &app_info;
  instance_create_info.enabledLayerCount = 0;
  instance_create_info.ppEnabledLayerNames = nullptr;
  instance_create_info.enabledExtensionCount = 0;
  instance_create_info.ppEnabledExtensionNames = nullptr;

  RAINBOW_CHECK_VK_RESULT(
      vkCreateInstance(&instance_create_info, nullptr, &instance_));

  uint32_t physical_device_count;
  RAINBOW_CHECK_VK_RESULT(
      vkEnumeratePhysicalDevices(instance_, &physical_device_count, nullptr));
  std::vector<VkPhysicalDevice> physical_devices{physical_device_count};
  RAINBOW_CHECK_VK_RESULT(vkEnumeratePhysicalDevices(
      instance_, &physical_device_count, physical_devices.data()));
  for (const auto& device : physical_devices) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    std::cout << properties.deviceName << std::endl;

    uint32_t queue_family_properties_count;
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queue_family_properties_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_family_properties{
        queue_family_properties_count};
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queue_family_properties_count, queue_family_properties.data());

    int queue_family_index = 0;
    for (const auto& properties : queue_family_properties) {
      std::cout << "  [" << queue_family_index << "]:";
      if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
        std::cout << " VK_QUEUE_GRAPHICS_BIT";
      }
      if ((properties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) {
        std::cout << " VK_QUEUE_COMPUTE_BIT";
      }
      if ((properties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) {
        std::cout << " VK_QUEUE_TRANSFER_BIT";
      }
      if ((properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) {
        std::cout << " VK_QUEUE_SPARSE_BINDING_BIT";
      }
      if ((properties.queueFlags & VK_QUEUE_PROTECTED_BIT) != 0) {
        std::cout << " VK_QUEUE_PROTECTED_BIT";
      }
      std::cout << std::endl;
      ++queue_family_index;
    }
  }
}

VulkanBackend::~VulkanBackend() { vkDestroyInstance(instance_, nullptr); }

void VulkanBackend::Prepare(const Scene& scene, size_t viewport_width,
                            size_t viewport_height) {}

void VulkanBackend::Render(const Camera& camera, Viewport* viewport) {}

}  // namespace rainbow
