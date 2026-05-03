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
}

void copy_buffer( vk::raii::Buffer &srcBuffer, vk::raii::Buffer &dstBuffer, vk::DeviceSize size,
                  std::shared_ptr<VulkanDevice> vulkanDevice, vk::raii::CommandPool &commandPool ) {
  // Begin command buffer
  std::unique_ptr<vk::raii::CommandBuffer> commandCopyBuffer =
      begin_single_time_commands( vulkanDevice, commandPool );

  // Copy buffer
  commandCopyBuffer->copyBuffer( *srcBuffer, *dstBuffer, vk::BufferCopy( 0, 0, size ) );

  // End command buffer
  end_single_time_commands( vulkanDevice, *commandCopyBuffer );
}

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
  vk::CommandBufferAllocateInfo allocInfo =
      Initializer::command_buffer_allocate_info( commandPool, 1 );

  // Allocate command buffer
  std::unique_ptr<vk::raii::CommandBuffer> commandBuffer =
      std::make_unique<vk::raii::CommandBuffer>(
          std::move( vulkanDevice->device.allocateCommandBuffers( allocInfo ).front() ) );

  // Begin command buffer
  vk::CommandBufferBeginInfo beginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
  commandBuffer->begin( beginInfo );

  // Return command buffer
  return commandBuffer;
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
}

}  // namespace Buffer
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
