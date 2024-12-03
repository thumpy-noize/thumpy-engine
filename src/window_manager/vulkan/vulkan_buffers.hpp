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

#include "vulkan/vulkan_helper.hpp"
#include "vulkan_device.hpp"
#include "vulkan_swap_chain.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Buffer {

void create_buffer( VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory, VulkanDevice *vulkanDevice );

void copy_buffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                  VulkanDevice *vulkanDevice, VkCommandPool &commandPool );

void create_framebuffers( VulkanSwapChain *swapChain, VkDevice device );

void create_vertex_buffer( std::vector<Vertex> vertices,
                           VulkanDevice *vulkanDevice, VkBuffer &vertexBuffer,
                           VkDeviceMemory &vertexBufferMemory,
                           VkCommandPool &commandPool );

void create_index_buffer( std::vector<uint16_t> indices,
                          VulkanDevice *vulkanDevice, VkBuffer &indexBuffer,
                          VkDeviceMemory &indexBufferMemory,
                          VkCommandPool &commandPool );

void create_texture_image( VulkanDevice *vulkanDevice,
                           TextureImage textureImage );

void create_image( uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, TextureImage textureImage,
                   VulkanDevice *vulkanDevice );

}  // namespace Buffer
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
