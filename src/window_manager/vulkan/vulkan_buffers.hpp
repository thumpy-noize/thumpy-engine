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
  vk::raii::Buffer buffer = nullptr;
  vk::raii::DeviceMemory memory = nullptr;

  // void destroy( VkDevice device ) {
  //   vkDestroyBuffer( device, buffer, nullptr );
  //   vkFreeMemory( device, memory, nullptr );
  // }
};

struct UniformBuffers {
  // We may be able to use a vector of buffers to replace these first 2 variables
  std::vector<vk::raii::Buffer> buffers;
  std::vector<vk::raii::DeviceMemory> memory;
  std::vector<void *> mapped;

  void clear() {
    buffers.clear();
    memory.clear();
    mapped.clear();
  }
};

void create_buffer( vk::DeviceSize size, vk::BufferUsageFlags usage,
                    vk::MemoryPropertyFlags properties, std::shared_ptr<Buffer> buffer,
                    std::shared_ptr<VulkanDevice> vulkanDevice );

void copy_buffer( std::shared_ptr<Buffer> srcBuffer, std::shared_ptr<Buffer> dstBuffer,
                  vk::DeviceSize size, std::shared_ptr<VulkanDevice> vulkanDevice,
                  vk::raii::CommandPool &commandPool );

// void create_framebuffers( VulkanSwapChain *swapChain, VkImageView depthImageView,
//                           VkImageView colorImageView, VkDevice device );

void create_vertex_buffer( std::vector<Vertex> vertices, std::shared_ptr<VulkanDevice> vulkanDevice,
                           std::shared_ptr<Buffer> vertexBuffer,
                           vk::raii::CommandPool &commandPool );

void create_index_buffer( std::vector<uint32_t> indices, std::shared_ptr<VulkanDevice> vulkanDevice,
                          std::shared_ptr<Buffer> indexBuffer, vk::raii::CommandPool &commandPool );

void create_uniform_buffers( std::shared_ptr<UniformBuffers> uniformBuffers,
                             std::shared_ptr<VulkanDevice> vulkanDevice, int maxFramesInFlight );

std::unique_ptr<vk::raii::CommandBuffer> begin_single_time_commands(
    std::shared_ptr<VulkanDevice> vulkanDevice, vk::raii::CommandPool &commandPool );

void end_single_time_commands(
    std::shared_ptr<VulkanDevice> vulkanDevice,
    vk::raii::CommandBuffer &commandBuffer /*, VkCommandPool commandPool*/ );

}  // namespace Buffer
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
