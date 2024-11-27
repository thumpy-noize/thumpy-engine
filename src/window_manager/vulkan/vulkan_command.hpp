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

#include "vulkan/vulkan_device.hpp"
namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

void create_command_pool(VulkanDevice *vulkanDevice,
                         VkCommandPool &commandPool);

}
} // namespace Windows
} // namespace Core
} // namespace Thumpy
