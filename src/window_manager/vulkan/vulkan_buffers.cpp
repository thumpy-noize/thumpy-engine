/**
 * @file vulkan_framebuffers.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief vulkan_framebuffer cpp file
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vulkan_buffers.hpp"

#include <vulkan/vulkan_core.h>

#include <cstring>
#include <string>

#include "logger_helper.hpp"
#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_initializers.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Buffer {

void create_buffer( VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                    VkBuffer &buffer, VkDeviceMemory &bufferMemory, VulkanDevice *vulkanDevice ) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if ( vkCreateBuffer( vulkanDevice->device, &bufferInfo, nullptr, &buffer ) != VK_SUCCESS ) {
    Logger::log( "Failed to create buffer!" );
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements( vulkanDevice->device, buffer, &memRequirements );

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      find_memory_type( vulkanDevice->physicalDevice, memRequirements.memoryTypeBits, properties );

  if ( vkAllocateMemory( vulkanDevice->device, &allocInfo, nullptr, &bufferMemory ) !=
       VK_SUCCESS ) {
    Logger::log( "Failed to allocate buffer memory!" );
  }

  vkBindBufferMemory( vulkanDevice->device, buffer, bufferMemory, 0 );
}

void copy_buffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                  VulkanDevice *vulkanDevice, VkCommandPool &commandPool ) {
  VkCommandBuffer commandBuffer = begin_single_time_commands( vulkanDevice->device, commandPool );

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer( commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion );

  end_single_time_commands( vulkanDevice, commandBuffer, commandPool );
}

void create_framebuffers( VulkanSwapChain *swapChain, VkImageView depthImageView,
                          VkImageView colorImageView, VkDevice device ) {
  swapChain->swapChainFramebuffers.resize( swapChain->swapChainImageViews.size() );

  for ( size_t i = 0; i < swapChain->swapChainImageViews.size(); i++ ) {
    std::array<VkImageView, 3> attachments = { colorImageView, depthImageView,
                                               swapChain->swapChainImageViews[i] };

    VkFramebufferCreateInfo framebufferInfo =
        Initializer::framebuffer_info( swapChain->renderPass, swapChain->extent, attachments );

    if ( vkCreateFramebuffer( device, &framebufferInfo, nullptr,
                              &swapChain->swapChainFramebuffers[i] ) != VK_SUCCESS ) {
      Logger::log( "Failed to create framebuffer!", Logger::CRITICAL );
    }
  }
}

void create_vertex_buffer( std::vector<Vertex> vertices, VulkanDevice *vulkanDevice,
                           Buffer *vertexBuffer, VkCommandPool &commandPool ) {
  VkDeviceSize bufferSize = sizeof( vertices[0] ) * vertices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  create_buffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory, vulkanDevice );

  void *data;
  vkMapMemory( vulkanDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data );
  memcpy( data, vertices.data(), (size_t)bufferSize );
  vkUnmapMemory( vulkanDevice->device, stagingBufferMemory );

  create_buffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer->buffer, vertexBuffer->memory,
                 vulkanDevice );

  copy_buffer( stagingBuffer, vertexBuffer->buffer, bufferSize, vulkanDevice, commandPool );

  vkDestroyBuffer( vulkanDevice->device, stagingBuffer, nullptr );
  vkFreeMemory( vulkanDevice->device, stagingBufferMemory, nullptr );
}

void create_index_buffer( std::vector<uint16_t> indices, VulkanDevice *vulkanDevice,
                          Buffer *indexBuffer, VkCommandPool &commandPool ) {
  VkDeviceSize bufferSize = sizeof( indices[0] ) * indices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  create_buffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory, vulkanDevice );

  void *data;
  vkMapMemory( vulkanDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data );
  memcpy( data, indices.data(), (size_t)bufferSize );
  vkUnmapMemory( vulkanDevice->device, stagingBufferMemory );

  create_buffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer->buffer, indexBuffer->memory,
                 vulkanDevice );

  copy_buffer( stagingBuffer, indexBuffer->buffer, bufferSize, vulkanDevice, commandPool );

  vkDestroyBuffer( vulkanDevice->device, stagingBuffer, nullptr );
  vkFreeMemory( vulkanDevice->device, stagingBufferMemory, nullptr );
}

VkCommandBuffer begin_single_time_commands( VkDevice device, VkCommandPool commandPool ) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers( device, &allocInfo, &commandBuffer );

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer( commandBuffer, &beginInfo );

  return commandBuffer;
}

void end_single_time_commands( VulkanDevice *vulkanDevice, VkCommandBuffer commandBuffer,
                               VkCommandPool commandPool ) {
  vkEndCommandBuffer( commandBuffer );

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit( vulkanDevice->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE );
  vkQueueWaitIdle( vulkanDevice->graphicsQueue );

  vkFreeCommandBuffers( vulkanDevice->device, commandPool, 1, &commandBuffer );
}

}  // namespace Buffer
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
