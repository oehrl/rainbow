#include "rainbow/backends/vulkan/vulkan_backend.hpp"
#include <iostream>
#include <rainbow/backends/vulkan/error.hpp>
#include "rainbow/backends/vulkan/vulkan.hpp"

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

  CreateDevice();

  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties);
  std::cout << "Heaps" << std::endl;
  for (uint32_t i = 0; i < memory_properties.memoryHeapCount; ++i) {
    std::cout << " [" << i << "]: " << memory_properties.memoryHeaps[i].size
              << std::endl;
  }
  std::cout << "Memeory types" << std::endl;
  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
    std::cout << " [" << i << "]->"
              << memory_properties.memoryTypes[i].heapIndex << ":";
    if (memory_properties.memoryTypes[i].propertyFlags &
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
      std::cout << " VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT";
    }
    if (memory_properties.memoryTypes[i].propertyFlags &
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
      std::cout << " VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT";
    }
    if (memory_properties.memoryTypes[i].propertyFlags &
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
      std::cout << " VK_MEMORY_PROPERTY_HOST_COHERENT_BIT";
    }
    if (memory_properties.memoryTypes[i].propertyFlags &
        VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
      std::cout << " VK_MEMORY_PROPERTY_HOST_CACHED_BIT";
    }
    if (memory_properties.memoryTypes[i].propertyFlags &
        VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) {
      std::cout << " VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT";
    }
    if (memory_properties.memoryTypes[i].propertyFlags &
        VK_MEMORY_PROPERTY_PROTECTED_BIT) {
      std::cout << " VK_MEMORY_PROPERTY_PROTECTED_BIT";
    }
    std::cout << std::endl;
  }
}

VulkanBackend::~VulkanBackend() { vkDestroyInstance(instance_, nullptr); }

void VulkanBackend::Prepare(const Scene& scene, size_t viewport_width,
                            size_t viewport_height) {}

void VulkanBackend::Render(const Camera& camera, Viewport* viewport) {}

void VulkanBackend::CreateDevice() {
  for (const auto& device : vulkan::GetPhysicalDevices(instance_)) {
    auto suitable_queue_families =
        vulkan::GetQueueFamilyIndicesForFlags(device, VK_QUEUE_COMPUTE_BIT);
    if (suitable_queue_families.size() > 0) {
      VkPhysicalDeviceProperties properties;
      vkGetPhysicalDeviceProperties(device, &properties);
      std::cout << "Using: " << properties.deviceName << std::endl;

      physical_device_ = device;
      queue_family_index_ = suitable_queue_families.front();
      break;
    }
  }

  float queue_priorities = 1.0f;

  VkDeviceQueueCreateInfo queue_create_info;
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.pNext = nullptr;
  queue_create_info.flags = 0;
  queue_create_info.queueFamilyIndex = queue_family_index_;
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &queue_priorities;

  VkDeviceCreateInfo device_create_info;
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pNext = nullptr;
  device_create_info.flags = 0;
  device_create_info.queueCreateInfoCount = 1;
  device_create_info.pQueueCreateInfos = &queue_create_info;
  device_create_info.enabledLayerCount = 0;
  device_create_info.ppEnabledLayerNames = nullptr;
  device_create_info.enabledExtensionCount = 0;
  device_create_info.ppEnabledExtensionNames = nullptr;
  device_create_info.pEnabledFeatures = nullptr;

  RAINBOW_CHECK_VK_RESULT(
      vkCreateDevice(physical_device_, &device_create_info, nullptr, &device_));
  vkGetDeviceQueue(device_, queue_family_index_, 0, &queue_);
}

}  // namespace rainbow
