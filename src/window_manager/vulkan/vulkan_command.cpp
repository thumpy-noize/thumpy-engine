/**
 * @file vulkan_command.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief vulkan_command cpp file
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vulkan_command.hpp"
#include "vulkan/vulkan_helper.hpp"
#include "vulkan/vulkan_initializers.hpp"
#include <stdexcept>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

void create_command_pool(VulkanDevice *vulkanDevice,
                         VkCommandPool &commandPool) {
  QueueFamilyIndices queueFamilyIndices =
      vulkanDevice->find_queue_families(vulkanDevice->physicalDevice);

  VkCommandPoolCreateInfo poolInfo =
      Initializer::pool_info(queueFamilyIndices.graphicsFamily.value());

  if (vkCreateCommandPool(vulkanDevice->device, &poolInfo, nullptr,
                          &commandPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
