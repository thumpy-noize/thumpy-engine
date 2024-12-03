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
                           TextureImage *textureImage,
                           VkCommandPool commandPool );

void create_image( uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, TextureImage *textureImage,
                   VulkanDevice *vulkanDevice );

VkCommandBuffer begin_single_time_commands( VkDevice device,
                                            VkCommandPool commandPool );

void end_single_time_commands( VulkanDevice *vulkanDevice,
                               VkCommandBuffer commandBuffer,
                               VkCommandPool commandPool );

void transition_image_layout( VkImage image, VkFormat format,
                              VkImageLayout oldLayout, VkImageLayout newLayout,
                              VulkanDevice *vulkanDevice,
                              VkCommandPool commandPool );

void copy_buffer_to_image( VkBuffer buffer, VkImage image, uint32_t width,
                           uint32_t height, VulkanDevice *vulkanDevice,
                           VkCommandPool commandPool );

}  // namespace Buffer
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
