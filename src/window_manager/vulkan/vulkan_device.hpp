/**
 * @file vulkan_device.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Contains vulkan device information and functions
 * @version 0.1
 * @date 2024-12-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include "vulkan_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

const std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

class VulkanDevice {
 public:
  // Require KHRSwapchian extention
  std::vector<const char *> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

  VulkanDevice( vk::raii::Instance &instance, vk::raii::SurfaceKHR &surface );

  /**
   * @brief Set up the vulkan device
   *
   * @param instance
   */
  void setup_device( vk::raii::Instance &instance, vk::raii::SurfaceKHR &surface );

  void pick_physical_device( vk::raii::Instance &instance );

  void create_logical_device( vk::raii::SurfaceKHR &surface );

  /**
   * @brief Checks if device is compatible with vulkan
   *
   * @param device device to check
   * @return Checks true if device is compatible
   */
  bool is_device_suitable( vk::raii::PhysicalDevice const &physicalDevice );

  vk::SampleCountFlagBits get_max_usable_sample_count();

  // Device variables
  vk::raii::PhysicalDevice physicalDevice = nullptr;
  vk::raii::Device device = nullptr;

  // Graphics queue
  vk::raii::Queue graphicsQueue = nullptr;
  uint32_t queueIndex = ~0;

  // Multisample antialiasing
  vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1;
};

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
