/**
 * @file vulkan_image.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Handles scripts related to VkImage
 * @version 0.1
 * @date 2024-12-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vulkan_image.hpp"

#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vulkan/vulkan_core.h>

#include "vulkan_buffers.hpp"
#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_initializers.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Image {

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

  Buffer::create_buffer( imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
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

void transition_image_layout( VkImage image, VkFormat format,
                              VkImageLayout oldLayout, VkImageLayout newLayout,
                              VulkanDevice *vulkanDevice,
                              VkCommandPool commandPool ) {
  VkCommandBuffer commandBuffer =
      Buffer::begin_single_time_commands( vulkanDevice->device, commandPool );

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

  Buffer::end_single_time_commands( vulkanDevice, commandBuffer, commandPool );
}

void copy_buffer_to_image( VkBuffer buffer, VkImage image, uint32_t width,
                           uint32_t height, VulkanDevice *vulkanDevice,
                           VkCommandPool commandPool ) {
  VkCommandBuffer commandBuffer =
      Buffer::begin_single_time_commands( vulkanDevice->device, commandPool );

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

  Buffer::end_single_time_commands( vulkanDevice, commandBuffer, commandPool );
}

}  // namespace Image
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy