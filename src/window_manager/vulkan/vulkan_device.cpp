#include "vulkan_device.hpp"
#include <stdexcept>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Device {

VulkanDevice::VulkanDevice(VkInstance instance) { setup_device(instance); }

void VulkanDevice::setup_device(VkInstance instance) {
  pick_physical_device(instance);
  create_logical_device();
}

void pick_physical_device(VkInstance instance) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  //   std::vector<VkPhysicalDevice> devices(deviceCount);
  //   vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

  //   for (const auto &device : devices) {
  //     if (is_device_suitable(device)) {
  //       physicalDevice_ = device;
  //       break;
  //     }
  //   }

  //   if (physicalDevice_ == VK_NULL_HANDLE) {
  //     throw std::runtime_error("failed to find a suitable GPU!");
  //   }
}

void VulkanDevice::create_logical_device() {}

} // namespace Device
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
