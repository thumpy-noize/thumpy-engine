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

void create_buffer( vk::DeviceSize size, vk::BufferUsageFlags usage,
                    vk::MemoryPropertyFlags properties, std::shared_ptr<Buffer> buffer,
                    std::shared_ptr<VulkanDevice> vulkanDevice ) {
  // Create buffer info
  vk::BufferCreateInfo bufferInfo{
      .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive };

  // Create buffer
  buffer->buffer = vk::raii::Buffer( vulkanDevice->device, bufferInfo );

  // Get memory requirements
  vk::MemoryRequirements memRequirements = buffer->buffer.getMemoryRequirements();

  // Create memory allocation info
  vk::MemoryAllocateInfo memoryAllocateInfo{
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = find_memory_type( vulkanDevice->physicalDevice,
                                           memRequirements.memoryTypeBits, properties ) };

  // Set vertex buffer memory
  buffer->memory = vk::raii::DeviceMemory( vulkanDevice->device, memoryAllocateInfo );

  // Bind memory
  buffer->buffer.bindMemory( *buffer->memory, 0 );

  // VkBufferCreateInfo bufferInfo{};
  // bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  // bufferInfo.size = size;
  // bufferInfo.usage = usage;
  // bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  // if ( vkCreateBuffer( vulkanDevice->device, &bufferInfo, nullptr, &buffer ) != VK_SUCCESS ) {
  //   Logger::log( "Failed to create buffer!" );
  // }

  // VkMemoryRequirements memRequirements;
  // vkGetBufferMemoryRequirements( vulkanDevice->device, buffer, &memRequirements );

  // VkMemoryAllocateInfo allocInfo{};
  // allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  // allocInfo.allocationSize = memRequirements.size;
  // allocInfo.memoryTypeIndex =
  //     find_memory_type( vulkanDevice->physicalDevice, memRequirements.memoryTypeBits, properties
  //     );

  // if ( vkAllocateMemory( vulkanDevice->device, &allocInfo, nullptr, &bufferMemory ) !=
  //      VK_SUCCESS ) {
  //   Logger::log( "Failed to allocate buffer memory!" );
  // }

  // vkBindBufferMemory( vulkanDevice->device, buffer, bufferMemory, 0 );
}

void copy_buffer( vk::raii::Buffer &srcBuffer, vk::raii::Buffer &dstBuffer, vk::DeviceSize size,
                  std::shared_ptr<VulkanDevice> vulkanDevice, vk::raii::CommandPool &commandPool ) {
  // // Create command buffer allocation info
  // vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool,
  //                                          .level = vk::CommandBufferLevel::ePrimary,
  //                                          .commandBufferCount = 1 };

  // // Allocate command buffer
  // vk::raii::CommandBuffer commandCopyBuffer =
  //     std::move( vulkanDevice->device.allocateCommandBuffers( allocInfo ).front() );

  // // Beging recording
  // commandCopyBuffer.begin(
  //     vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit } );

  // Begin command buffer
  std::unique_ptr<vk::raii::CommandBuffer> commandCopyBuffer =
      begin_single_time_commands( vulkanDevice, commandPool );

  // Copy buffer
  commandCopyBuffer->copyBuffer( *srcBuffer, *dstBuffer, vk::BufferCopy( 0, 0, size ) );

  // End command buffer
  end_single_time_commands( vulkanDevice, *commandCopyBuffer );

  // // End recording
  // commandCopyBuffer.end();

  // // Submit command buffer
  // vulkanDevice->graphicsQueue.submit(
  //     vk::SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer }, nullptr
  //     );
  // vulkanDevice->graphicsQueue.waitIdle();

  // --- NON RAII Deprecated ---

  // VkCommandBuffer commandBuffer = begin_single_time_commands( vulkanDevice->device, commandPool
  // );

  // VkBufferCopy copyRegion{};
  // copyRegion.size = size;
  // vkCmdCopyBuffer( commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion );

  // end_single_time_commands( vulkanDevice, commandBuffer, commandPool );
}

// void create_framebuffers( VulkanSwapChain *swapChain, VkImageView depthImageView,
//                           VkImageView colorImageView, VkDevice device ) {
//   swapChain->swapChainFramebuffers.resize( swapChain->swapChainImageViews.size() );

//   for ( size_t i = 0; i < swapChain->swapChainImageViews.size(); i++ ) {
//     std::array<VkImageView, 3> attachments = { colorImageView, depthImageView,
//                                                swapChain->swapChainImageViews[i] };

//     VkFramebufferCreateInfo framebufferInfo =
//         Initializer::framebuffer_info( swapChain->renderPass, swapChain->extent, attachments );

//     if ( vkCreateFramebuffer( device, &framebufferInfo, nullptr,
//                               &swapChain->swapChainFramebuffers[i] ) != VK_SUCCESS ) {
//       Logger::log( "Failed to create framebuffer!", Logger::CRITICAL );
//     }
//   }
// }

void create_vertex_buffer( std::vector<Vertex> vertices, std::shared_ptr<VulkanDevice> vulkanDevice,
                           std::shared_ptr<Buffer> vertexBuffer,
                           vk::raii::CommandPool &commandPool ) {
  Logger::log( "Creating vertex buffer...", Logger::DEBUG );

  // Get buffer size
  vk::DeviceSize bufferSize = sizeof( vertices[0] ) * vertices.size();

  // Create staging buffer
  std::shared_ptr<Buffer> stagingBuffer = std::make_shared<Buffer>();
  create_buffer(
      bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
      stagingBuffer, vulkanDevice );

  // Map staging buffer
  void *dataStaging = stagingBuffer->memory.mapMemory( 0, bufferSize );
  memcpy( dataStaging, vertices.data(), bufferSize );
  stagingBuffer->memory.unmapMemory();

  // Create vertex buffer
  create_buffer( bufferSize,
                 vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vulkanDevice );

  // Copy staging buffer to vertex buffer
  copy_buffer( stagingBuffer->buffer, vertexBuffer->buffer, bufferSize, vulkanDevice, commandPool );

  // ########################
  // ###### Deprecated ######
  // ########################

  // VkDeviceSize bufferSize = sizeof( vertices[0] ) * vertices.size();

  // VkBuffer stagingBuffer;
  // VkDeviceMemory stagingBufferMemory;
  // create_buffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
  //                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  //                stagingBuffer, stagingBufferMemory, vulkanDevice );

  // void *data;
  // vkMapMemory( vulkanDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data );
  // memcpy( data, vertices.data(), (size_t)bufferSize );
  // vkUnmapMemory( vulkanDevice->device, stagingBufferMemory );

  // create_buffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
  // VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  //                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer->buffer, vertexBuffer->memory,
  //                vulkanDevice );

  // copy_buffer( stagingBuffer, vertexBuffer->buffer, bufferSize, vulkanDevice, commandPool );

  // vkDestroyBuffer( vulkanDevice->device, stagingBuffer, nullptr );
  // vkFreeMemory( vulkanDevice->device, stagingBufferMemory, nullptr );
}

void create_index_buffer( std::vector<uint32_t> indices, std::shared_ptr<VulkanDevice> vulkanDevice,
                          std::shared_ptr<Buffer> indexBuffer,
                          vk::raii::CommandPool &commandPool ) {
  Logger::log( "Creating index buffer...", Logger::DEBUG );

  // Get index buffer size
  vk::DeviceSize bufferSize = sizeof( indices[0] ) * indices.size();

  // Create staging buffer
  std::shared_ptr<Buffer> stagingBuffer = std::make_shared<Buffer>();
  create_buffer(
      bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
      stagingBuffer, vulkanDevice );

  // Map staging buffer
  void *dataStaging = stagingBuffer->memory.mapMemory( 0, bufferSize );
  memcpy( dataStaging, indices.data(), (size_t)bufferSize );
  stagingBuffer->memory.unmapMemory();

  // Create index buffer
  // Create vertex buffer
  create_buffer( bufferSize,
                 vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, vulkanDevice );

  // Copy staging buffer to index buffer
  copy_buffer( stagingBuffer->buffer, indexBuffer->buffer, bufferSize, vulkanDevice, commandPool );

  //   VkDeviceSize bufferSize = sizeof( indices[0] ) * indices.size();

  //   VkBuffer stagingBuffer;
  //   VkDeviceMemory stagingBufferMemory;
  //   create_buffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
  //                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  //                  stagingBuffer, stagingBufferMemory, vulkanDevice );

  //   void *data;
  //   vkMapMemory( vulkanDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data );
  //   memcpy( data, indices.data(), (size_t)bufferSize );
  //   vkUnmapMemory( vulkanDevice->device, stagingBufferMemory );

  //   create_buffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
  //   VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
  //                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer->buffer, indexBuffer->memory,
  //                  vulkanDevice );

  //   copy_buffer( stagingBuffer, indexBuffer->buffer, bufferSize, vulkanDevice, commandPool );

  //   vkDestroyBuffer( vulkanDevice->device, stagingBuffer, nullptr );
  //   vkFreeMemory( vulkanDevice->device, stagingBufferMemory, nullptr );
}

void create_uniform_buffers( std::shared_ptr<UniformBuffers> uniformBuffers,
                             std::shared_ptr<VulkanDevice> vulkanDevice, int maxFramesInFlight ) {
  // Clear buffer
  uniformBuffers->clear();

  // For frames in flight
  for ( size_t i = 0; i < maxFramesInFlight; i++ ) {
    // Get buffer size
    vk::DeviceSize bufferSize = sizeof( UniformBufferObject );

    // Create buffer
    std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>();
    create_buffer(
        bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        buffer, vulkanDevice );

    // Add created buffer to uniform buffers
    uniformBuffers->buffers.emplace_back( std::move( buffer->buffer ) );
    uniformBuffers->memory.emplace_back( std::move( buffer->memory ) );

    // Map memory
    uniformBuffers->mapped.emplace_back( uniformBuffers->memory[i].mapMemory( 0, bufferSize ) );
  }
}

std::unique_ptr<vk::raii::CommandBuffer> begin_single_time_commands(
    std::shared_ptr<VulkanDevice> vulkanDevice, vk::raii::CommandPool &commandPool ) {
  // Create command buffer allocation info
  vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool,
                                           .level = vk::CommandBufferLevel::ePrimary,
                                           .commandBufferCount = 1 };

  // Allocate command buffer
  std::unique_ptr<vk::raii::CommandBuffer> commandBuffer =
      std::make_unique<vk::raii::CommandBuffer>(
          std::move( vulkanDevice->device.allocateCommandBuffers( allocInfo ).front() ) );

  // Begin command buffer
  vk::CommandBufferBeginInfo beginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
  commandBuffer->begin( beginInfo );

  // Return command buffer
  return commandBuffer;

  // VkCommandBuffer begin_single_time_commands( VkDevice device, VkCommandPool commandPool ) {
  //   VkCommandBufferAllocateInfo allocInfo{};
  //   allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  //   allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  //   allocInfo.commandPool = commandPool;
  //   allocInfo.commandBufferCount = 1;

  //   VkCommandBuffer commandBuffer;
  //   vkAllocateCommandBuffers( device, &allocInfo, &commandBuffer );

  //   VkCommandBufferBeginInfo beginInfo{};
  //   beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  //   beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  //   vkBeginCommandBuffer( commandBuffer, &beginInfo );

  //   return commandBuffer;
}

void end_single_time_commands( std::shared_ptr<VulkanDevice> vulkanDevice,
                               vk::raii::CommandBuffer &commandBuffer ) {
  // End command buffer
  commandBuffer.end();

  // Create submit info
  vk::SubmitInfo submitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };

  // Submit to queue
  vulkanDevice->graphicsQueue.submit( submitInfo, nullptr );
  // Wait for queue
  vulkanDevice->graphicsQueue.waitIdle();

  // void end_single_time_commands( VulkanDevice *vulkanDevice, VkCommandBuffer commandBuffer,
  //                                VkCommandPool commandPool ) {
  //   vkEndCommandBuffer( commandBuffer );

  //   VkSubmitInfo submitInfo{};
  //   submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  //   submitInfo.commandBufferCount = 1;
  //   submitInfo.pCommandBuffers = &commandBuffer;

  //   vkQueueSubmit( vulkanDevice->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE );
  //   vkQueueWaitIdle( vulkanDevice->graphicsQueue );

  //   vkFreeCommandBuffers( vulkanDevice->device, commandPool, 1, &commandBuffer );
}

}  // namespace Buffer
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
