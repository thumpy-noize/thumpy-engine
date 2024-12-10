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
#include <sys/types.h>

#include <cstdint>
#include <string>
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <vulkan/vulkan_core.h>

#include <stdexcept>

#include "logger_helper.hpp"
#include "vulkan_buffers.hpp"
#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_image.hpp"
#include "vulkan_initializers.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

namespace Image {

void create_image( uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                   VulkanImage *textureImage, VulkanDevice *vulkanDevice ) {
  VkImageCreateInfo imageInfo = Initializer::image_info( width, height, format, tiling, usage );
  imageInfo.mipLevels = mipLevels;

  if ( vkCreateImage( vulkanDevice->device, &imageInfo, nullptr, &textureImage->image ) !=
       VK_SUCCESS ) {
    Logger::log( "Failed to create image!", Logger::CRITICAL );
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements( vulkanDevice->device, textureImage->image, &memRequirements );

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      find_memory_type( vulkanDevice->physicalDevice, memRequirements.memoryTypeBits, properties );

  if ( vkAllocateMemory( vulkanDevice->device, &allocInfo, nullptr, &textureImage->imageMemory ) !=
       VK_SUCCESS ) {
    Logger::log( "failed to allocate image memory!", Logger::CRITICAL );
  }

  vkBindImageMemory( vulkanDevice->device, textureImage->image, textureImage->imageMemory, 0 );
}

void create_texture_image( VulkanDevice *vulkanDevice, VulkanTextureImage *textureImage,
                           VkCommandPool commandPool, std::string filePath ) {
  Texture *texture = load_texture( filePath );
  textureImage->mipLevels = static_cast<uint32_t>( std::floor(
                                std::log2( std::max( texture->height, texture->width ) ) ) ) +
                            1;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  Buffer::create_buffer( texture->imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer, stagingBufferMemory, vulkanDevice );

  void *data;
  vkMapMemory( vulkanDevice->device, stagingBufferMemory, 0, texture->imageSize, 0, &data );
  memcpy( data, texture->pixels, static_cast<size_t>( texture->imageSize ) );
  vkUnmapMemory( vulkanDevice->device, stagingBufferMemory );

  free_texture( texture );

  create_image( texture->width, texture->height, textureImage->mipLevels, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, vulkanDevice );

  transition_image_layout( textureImage->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vulkanDevice, commandPool,
                           textureImage->mipLevels );

  copy_buffer_to_image( stagingBuffer, textureImage->image, static_cast<uint32_t>( texture->width ),
                        static_cast<uint32_t>( texture->height ), vulkanDevice, commandPool );

  // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
  //  transition_image_layout( textureImage->image, VK_FORMAT_R8G8B8A8_SRGB,
  //                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
  //                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, vulkanDevice, commandPool,
  //                           1 );

  vkDestroyBuffer( vulkanDevice->device, stagingBuffer, nullptr );
  vkFreeMemory( vulkanDevice->device, stagingBufferMemory, nullptr );
  generate_mipmaps( textureImage->image, VK_FORMAT_R8G8B8A8_SRGB, texture->width, texture->height,
                    textureImage->mipLevels, vulkanDevice, commandPool );
}

void transition_image_layout( VkImage image, VkFormat format, VkImageLayout oldLayout,
                              VkImageLayout newLayout, VulkanDevice *vulkanDevice,
                              VkCommandPool commandPool, uint32_t mipLevels ) {
  VkCommandBuffer commandBuffer =
      Buffer::begin_single_time_commands( vulkanDevice->device, commandPool );

  VkImageMemoryBarrier barrier = Initializer::image_memory_barrier( image, oldLayout, newLayout );

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

  vkCmdPipelineBarrier( commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1,
                        &barrier );

  Buffer::end_single_time_commands( vulkanDevice, commandBuffer, commandPool );
}

void copy_buffer_to_image( VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
                           VulkanDevice *vulkanDevice, VkCommandPool commandPool ) {
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

  vkCmdCopyBufferToImage( commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                          &region );

  Buffer::end_single_time_commands( vulkanDevice, commandBuffer, commandPool );
}

VkImageView create_image_view( VkDevice device, VkImage image, VkFormat format,
                               VkImageAspectFlags aspectFlags, uint32_t mipLevels ) {
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  if ( vkCreateImageView( device, &viewInfo, nullptr, &imageView ) != VK_SUCCESS ) {
    throw std::runtime_error( "failed to create image view!" );
  }

  return imageView;
}

void create_texture_image_view( VkDevice device, VulkanTextureImage *textureImage ) {
  // Create image view info
  textureImage->imageView = create_image_view( device, textureImage->image, VK_FORMAT_R8G8B8A8_SRGB,
                                               VK_IMAGE_ASPECT_COLOR_BIT, textureImage->mipLevels );
}

void create_texture_sampler( VulkanDevice *vulkanDevice, VulkanTextureImage *textureImage ) {
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties( vulkanDevice->physicalDevice, &properties );

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;

  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;

  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.minLod = 0.0f;  // Optional
  samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
  samplerInfo.mipLodBias = 0.0f;  // Optional

  if ( vkCreateSampler( vulkanDevice->device, &samplerInfo, nullptr, &textureImage->sampler ) !=
       VK_SUCCESS ) {
    Logger::log( "Failed to create texture sampler!", Logger::CRITICAL );
  }
}

void create_depth_resources( VulkanImage *depthBuffer, VulkanDevice *vulkanDevice,
                             VkExtent2D swapChainExtent ) {
  VkFormat depthFormat = find_depth_format( vulkanDevice->physicalDevice );

  create_image( swapChainExtent.width, swapChainExtent.height, 1, depthFormat,
                VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthBuffer, vulkanDevice );

  depthBuffer->imageView = create_image_view( vulkanDevice->device, depthBuffer->image, depthFormat,
                                              VK_IMAGE_ASPECT_DEPTH_BIT, 1 );
}

VkFormat find_supported_format( const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice ) {
  for ( VkFormat format : candidates ) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties( physicalDevice, format, &props );

    if ( tiling == VK_IMAGE_TILING_LINEAR &&
         ( props.linearTilingFeatures & features ) == features ) {
      return format;
    } else if ( tiling == VK_IMAGE_TILING_OPTIMAL &&
                ( props.optimalTilingFeatures & features ) == features ) {
      return format;
    }
  }

  Logger::log( "Failed to find supported format!", Logger::CRITICAL );
  throw std::runtime_error( "Failed to find supported format!" );
}

VkFormat find_depth_format( VkPhysicalDevice physicalDevice ) {
  return find_supported_format(
      { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
      VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, physicalDevice );
}

bool has_stencil_component( VkFormat format ) {
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void generate_mipmaps( VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight,
                       uint32_t mipLevels, VulkanDevice *vulkanDevice, VkCommandPool commandPool ) {
  // Check if image format supports linear blitting
  VkFormatProperties formatProperties;
  vkGetPhysicalDeviceFormatProperties( vulkanDevice->physicalDevice, imageFormat,
                                       &formatProperties );
  if ( !( formatProperties.optimalTilingFeatures &
          VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT ) ) {
    Logger::log( "Texture image format does not support linear blitting!", Logger::CRITICAL );
  }

  VkCommandBuffer commandBuffer =
      Buffer::begin_single_time_commands( vulkanDevice->device, commandPool );

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image = image;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.subresourceRange.levelCount = 1;

  int32_t mipWidth = texWidth;
  int32_t mipHeight = texHeight;

  for ( uint32_t i = 1; i < mipLevels; i++ ) {
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier( commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                          VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier );

    VkImageBlit blit{};
    blit.srcOffsets[0] = { 0, 0, 0 };
    blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = { 0, 0, 0 };
    blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage( commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR );

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier( commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                          &barrier );

    if ( mipWidth > 1 ) mipWidth /= 2;
    if ( mipHeight > 1 ) mipHeight /= 2;
  }

  barrier.subresourceRange.baseMipLevel = mipLevels - 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier( commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                        &barrier );

  Buffer::end_single_time_commands( vulkanDevice, commandBuffer, commandPool );
}

}  // namespace Image
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy