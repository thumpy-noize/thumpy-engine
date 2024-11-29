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

#include "vulkan/vulkan_swap_chain.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Buffer {

void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer &buffer,
                   VkDeviceMemory &bufferMemory, VulkanDevice *vulkanDevice);

void create_framebuffers(VulkanSwapChain *swapChain, VkDevice device);

void create_vertex_buffer(std::vector<Vertex> vertices,
                          VulkanDevice *vulkanDevice, VkBuffer &vertexBuffer,
                          VkDeviceMemory &vertexBufferMemory);

} // namespace Buffer
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
