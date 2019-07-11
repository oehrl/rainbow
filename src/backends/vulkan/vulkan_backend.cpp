#include "rainbow/backends/vulkan/vulkan_backend.hpp"
#include <iostream>
#include <rainbow/backends/vulkan/error.hpp>
#include "rainbow/backends/vulkan/vulkan.hpp"
#include "rainbow/scene.hpp"

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
}

VulkanBackend::~VulkanBackend() { vkDestroyInstance(instance_, nullptr); }

void VulkanBackend::Prepare(const Scene& scene, size_t viewport_width,
                            size_t viewport_height) {
  VkDeviceSize required_memory = 0;
  required_memory += scene.GetVertexPositions().size() * sizeof(Vector3);
  required_memory += scene.GetVertexNormals().size() * sizeof(Vector3);
  required_memory += scene.GetTriangleCount() * sizeof(TriangleReference);
  required_memory += scene.GetMaterials().size() * sizeof(Material);
  std::cout << "Memory requirement: " << required_memory << std::endl;

  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties);
  const auto memory_index = vulkan::FindMemoryTypeIndex(
      memory_properties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VkMemoryAllocateInfo allocate_info;
  allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocate_info.pNext = nullptr;
  allocate_info.allocationSize = required_memory;
  allocate_info.memoryTypeIndex = memory_index;
  RAINBOW_CHECK_VK_RESULT(
      vkAllocateMemory(device_, &allocate_info, nullptr, &memory_));

  VkBufferCreateInfo buffer_create_info;
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.pNext = nullptr;
  buffer_create_info.flags = 0;
  buffer_create_info.size = scene.GetVertexPositions().size() * sizeof(Vector3);
  buffer_create_info.usage =
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  buffer_create_info.queueFamilyIndexCount = 1;
  buffer_create_info.pQueueFamilyIndices = &queue_family_index_;
  RAINBOW_CHECK_VK_RESULT(
      vkCreateBuffer(device_, &buffer_create_info, nullptr, &vertex_buffer_));
  RAINBOW_CHECK_VK_RESULT(
      vkBindBufferMemory(device_, vertex_buffer_, memory_, 0));
}

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
