#pragma once

#include <vulkan/vulkan_core.h>
namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Device {

class VulkanDevice {
public:
  VulkanDevice(VkInstance instance);

  void setup_device(VkInstance instance);

  void pick_physical_device(VkInstance instance);
  void create_logical_device();

  bool is_device_suitable(VkPhysicalDevice device);

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
};

} // namespace Device
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
