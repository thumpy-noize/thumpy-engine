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
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_swap_chain.hpp"
namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

void create_command_pool( VulkanDevice *vulkanDevice,
                          VkCommandPool &commandPool );

void create_command_buffer( std::vector<VkCommandBuffer> &commandBuffers,
                            VkCommandPool commandPool, VkDevice device,
                            int max_frames_in_flight );

void record_command_buffer( VkCommandBuffer commandBuffer, uint32_t imageIndex,
                            VulkanSwapChain *swapChain,
                            VulkanPipeline pipeline );

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
