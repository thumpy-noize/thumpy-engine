/**
 * @file vulkan_framebuffers.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief This is where we work with Vulkan's framebuffers
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_swap_chain.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Buffer {

struct Buffer {
  VkBuffer buffer;
  VkDeviceMemory memory;

  void destroy( VkDevice device ) {
    vkDestroyBuffer( device, buffer, nullptr );
    vkFreeMemory( device, memory, nullptr );
  }
};

void create_buffer( VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                    VkBuffer &buffer, VkDeviceMemory &bufferMemory, VulkanDevice *vulkanDevice );

void copy_buffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                  VulkanDevice *vulkanDevice, VkCommandPool &commandPool );

void create_framebuffers( VulkanSwapChain *swapChain, VkImageView depthImageView,
                          VkImageView colorImageView, VkDevice device );

void create_vertex_buffer( std::vector<Vertex> vertices, VulkanDevice *vulkanDevice,
                           Buffer *vertexBuffer, VkCommandPool &commandPool );

void create_index_buffer( std::vector<uint16_t> indices, VulkanDevice *vulkanDevice,
                          Buffer *indexBuffer, VkCommandPool &commandPool );

VkCommandBuffer begin_single_time_commands( VkDevice device, VkCommandPool commandPool );

void end_single_time_commands( VulkanDevice *vulkanDevice, VkCommandBuffer commandBuffer,
                               VkCommandPool commandPool );

}  // namespace Buffer
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
