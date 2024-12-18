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

#include "vulkan_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

const std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

class VulkanDevice {
 public:
  VulkanDevice( VkInstance instance, VkSurfaceKHR surface );

  /**
   * @brief Set up the vulkan device
   *
   * @param instance
   */
  void setup_device( VkInstance instance );

  void pick_physical_device( VkInstance instance );
  void create_logical_device();

  /**
   * @brief Checks if device is compatible with vulkan
   *
   * @param device device to check
   * @return Checks true if device is compatible
   */
  bool is_device_suitable( VkPhysicalDevice device );

  QueueFamilyIndices find_queue_families( VkPhysicalDevice device );
  bool check_device_extension_support( VkPhysicalDevice device );

  SwapChainSupportDetails query_swap_chain_support( VkPhysicalDevice device );

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;

  VkQueue graphicsQueue;
  VkQueue presentQueue;

  VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

 private:
  VkSurfaceKHR surface_;
};

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
