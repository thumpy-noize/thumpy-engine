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

#include "vulkan/vulkan_device.hpp"
#include "vulkan/vulkan_swap_chain.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Buffer {

void create_framebuffers(VulkanSwapChain *swapChain, VkDevice device);

class VertexBuffer {
public:
  void create_vertex_buffer(std::vector<Vertex> vertices_,
                            VulkanDevice *vulkanDevice);

  uint32_t find_memory_type(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties,
                            VkPhysicalDevice physicalDevice);

public:
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
};

} // namespace Buffer
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
