/**
 * @file vulkan_command.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief This scrip handles Vulkan command pool & buffers
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <vulkan/vulkan_core.h>

#include "vulkan_device.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Construct {

void create_instance( VkInstance &instance );

void create_command_pool( VulkanDevice *vulkanDevice,
                          VkCommandPool &commandPool );

void create_command_buffer( std::vector<VkCommandBuffer> &commandBuffers,
                            VkCommandPool commandPool, VkDevice device,
                            int maxFramesInFlight );
}  // namespace Construct
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
