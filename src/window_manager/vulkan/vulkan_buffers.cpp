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

#include <stdexcept>

#include "vulkan/vulkan_helper.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vulkan/vulkan_core.h>

#include <cstring>

#include "logger_helper.hpp"
#include "vulkan/vulkan_device.hpp"
#include "vulkan_initializers.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Buffer {

void create_buffer( VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory, VulkanDevice *vulkanDevice ) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if ( vkCreateBuffer( vulkanDevice->device, &bufferInfo, nullptr, &buffer ) !=
       VK_SUCCESS ) {
    Logger::log( "Failed to create buffer!" );
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements( vulkanDevice->device, buffer,
                                 &memRequirements );

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      find_memory_type( vulkanDevice->physicalDevice,
                        memRequirements.memoryTypeBits, properties );

  if ( vkAllocateMemory( vulkanDevice->device, &allocInfo, nullptr,
                         &bufferMemory ) != VK_SUCCESS ) {
    Logger::log( "Failed to allocate buffer memory!" );
  }

  vkBindBufferMemory( vulkanDevice->device, buffer, bufferMemory, 0 );
}

void copy_buffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                  VulkanDevice *vulkanDevice, VkCommandPool &commandPool ) {
  VkCommandBuffer commandBuffer =
      begin_single_time_commands( vulkanDevice->device, commandPool );

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer( commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion );

  end_single_time_commands( vulkanDevice, commandBuffer, commandPool );
}

void create_framebuffers( VulkanSwapChain *swapChain, VkDevice device ) {
  swapChain->swapChainFramebuffers.resize(
      swapChain->swapChainImageViews.size() );

  for ( size_t i = 0; i < swapChain->swapChainImageViews.size(); i++ ) {
    VkImageView attachments[] = { swapChain->swapChainImageViews[i] };

    VkFramebufferCreateInfo framebufferInfo = Initializer::framebuffer_info(
        swapChain->renderPass, swapChain->extent, attachments );

    if ( vkCreateFramebuffer( device, &framebufferInfo, nullptr,
                              &swapChain->swapChainFramebuffers[i] ) !=
         VK_SUCCESS ) {
      Logger::log( "Failed to create framebuffer!", Logger::CRITICAL );
    }
  }
}

void create_vertex_buffer( std::vector<Vertex> vertices,
                           VulkanDevice *vulkanDevice, VkBuffer &vertexBuffer,
                           VkDeviceMemory &vertexBufferMemory,
                           VkCommandPool &commandPool ) {
  VkDeviceSize bufferSize = sizeof( vertices[0] ) * vertices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  create_buffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory, vulkanDevice );

  void *data;
  vkMapMemory( vulkanDevice->device, stagingBufferMemory, 0, bufferSize, 0,
               &data );
  memcpy( data, vertices.data(), (size_t)bufferSize );
  vkUnmapMemory( vulkanDevice->device, stagingBufferMemory );

  create_buffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory,
      vulkanDevice );

  copy_buffer( stagingBuffer, vertexBuffer, bufferSize, vulkanDevice,
               commandPool );

  vkDestroyBuffer( vulkanDevice->device, stagingBuffer, nullptr );
  vkFreeMemory( vulkanDevice->device, stagingBufferMemory, nullptr );
}

void create_index_buffer( std::vector<uint16_t> indices,
                          VulkanDevice *vulkanDevice, VkBuffer &indexBuffer,
                          VkDeviceMemory &indexBufferMemory,
                          VkCommandPool &commandPool ) {
  VkDeviceSize bufferSize = sizeof( indices[0] ) * indices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  create_buffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory, vulkanDevice );

  void *data;
  vkMapMemory( vulkanDevice->device, stagingBufferMemory, 0, bufferSize, 0,
               &data );
  memcpy( data, indices.data(), (size_t)bufferSize );
  vkUnmapMemory( vulkanDevice->device, stagingBufferMemory );

  create_buffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory,
      vulkanDevice );

  copy_buffer( stagingBuffer, indexBuffer, bufferSize, vulkanDevice,
               commandPool );

  vkDestroyBuffer( vulkanDevice->device, stagingBuffer, nullptr );
  vkFreeMemory( vulkanDevice->device, stagingBufferMemory, nullptr );
}

void create_texture_image( VulkanDevice *vulkanDevice,
                           TextureImage *textureImage,
                           VkCommandPool commandPool ) {
  Logger::log( "Loading texture: " + get_texture_path() + "vj_swirl.png",
               Logger::INFO );
  int texWidth, texHeight, texChannels;
  stbi_uc *pixels =
      stbi_load( std::string( get_texture_path() + "vj_swirl.png" ).c_str(),
                 &texWidth, &texHeight, &texChannels, STBI_rgb_alpha );
  VkDeviceSize imageSize = texWidth * texHeight * 4;

  if ( !pixels ) {
    Logger::log( "Failed to load texture image!", Logger::CRITICAL );
  }

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  create_buffer( imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory, vulkanDevice );

  void *data;
  vkMapMemory( vulkanDevice->device, stagingBufferMemory, 0, imageSize, 0,
               &data );
  memcpy( data, pixels, static_cast<size_t>( imageSize ) );
  vkUnmapMemory( vulkanDevice->device, stagingBufferMemory );

  stbi_image_free( pixels );

  create_image(
      texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, vulkanDevice );

  transition_image_layout(
      textureImage->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vulkanDevice, commandPool );

  copy_buffer_to_image(
      stagingBuffer, textureImage->image, static_cast<uint32_t>( texWidth ),
      static_cast<uint32_t>( texHeight ), vulkanDevice, commandPool );

  transition_image_layout( textureImage->image, VK_FORMAT_R8G8B8A8_SRGB,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                           vulkanDevice, commandPool );

  vkDestroyBuffer( vulkanDevice->device, stagingBuffer, nullptr );
  vkFreeMemory( vulkanDevice->device, stagingBufferMemory, nullptr );
}

void create_image( uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, TextureImage *textureImage,
                   VulkanDevice *vulkanDevice ) {
  VkImageCreateInfo imageInfo =
      Initializer::image_info( width, height, format, tiling, usage );

  if ( vkCreateImage( vulkanDevice->device, &imageInfo, nullptr,
                      &textureImage->image ) != VK_SUCCESS ) {
    Logger::log( "Failed to create image!", Logger::CRITICAL );
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements( vulkanDevice->device, textureImage->image,
                                &memRequirements );

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      find_memory_type( vulkanDevice->physicalDevice,
                        memRequirements.memoryTypeBits, properties );

  if ( vkAllocateMemory( vulkanDevice->device, &allocInfo, nullptr,
                         &textureImage->imageMemory ) != VK_SUCCESS ) {
    Logger::log( "failed to allocate image memory!", Logger::CRITICAL );
  }

  vkBindImageMemory( vulkanDevice->device, textureImage->image,
                     textureImage->imageMemory, 0 );
}

VkCommandBuffer begin_single_time_commands( VkDevice device,
                                            VkCommandPool commandPool ) {
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

void end_single_time_commands( VulkanDevice *vulkanDevice,
                               VkCommandBuffer commandBuffer,
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

void transition_image_layout( VkImage image, VkFormat format,
                              VkImageLayout oldLayout, VkImageLayout newLayout,
                              VulkanDevice *vulkanDevice,
                              VkCommandPool commandPool ) {
  VkCommandBuffer commandBuffer =
      begin_single_time_commands( vulkanDevice->device, commandPool );

  VkImageMemoryBarrier barrier =
      Initializer::image_memory_barrier( image, oldLayout, newLayout );

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
       newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
              newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    throw std::invalid_argument( "Unsupported layout transition!" );
  }

  vkCmdPipelineBarrier( commandBuffer, sourceStage, destinationStage, 0, 0,
                        nullptr, 0, nullptr, 1, &barrier );

  end_single_time_commands( vulkanDevice, commandBuffer, commandPool );
}

void copy_buffer_to_image( VkBuffer buffer, VkImage image, uint32_t width,
                           uint32_t height, VulkanDevice *vulkanDevice,
                           VkCommandPool commandPool ) {
  VkCommandBuffer commandBuffer =
      begin_single_time_commands( vulkanDevice->device, commandPool );

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = { 0, 0, 0 };
  region.imageExtent = { width, height, 1 };

  vkCmdCopyBufferToImage( commandBuffer, buffer, image,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region );

  end_single_time_commands( vulkanDevice, commandBuffer, commandPool );
}

}  // namespace Buffer
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
