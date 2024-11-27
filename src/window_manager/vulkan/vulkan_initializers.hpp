/**
 * @file vulkan_initializers.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Initializers for Vulkan structures used by examples
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <vulkan/vulkan_core.h>
namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Initializer {

inline VkApplicationInfo application_info() {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Thumpy Engine Vulkan Window";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Thumpy Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;
  return appInfo;
}

inline VkCommandBufferAllocateInfo
command_buffer_allocate_info(VkCommandPool commandPool, uint32_t bufferCount) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = bufferCount;
  return allocInfo;
}
} // namespace Initializer
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
