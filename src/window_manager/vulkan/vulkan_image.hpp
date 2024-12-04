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

#pragma once

#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Image {

void create_texture_image( VulkanDevice *vulkanDevice,
                           TextureImage *textureImage,
                           VkCommandPool commandPool );

void create_image( uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, TextureImage *textureImage,
                   VulkanDevice *vulkanDevice );

void transition_image_layout( VkImage image, VkFormat format,
                              VkImageLayout oldLayout, VkImageLayout newLayout,
                              VulkanDevice *vulkanDevice,
                              VkCommandPool commandPool );

void copy_buffer_to_image( VkBuffer buffer, VkImage image, uint32_t width,
                           uint32_t height, VulkanDevice *vulkanDevice,
                           VkCommandPool commandPool );

VkImageView create_image_view( VkDevice device, VkImage image,
                               VkFormat format );

void create_texture_image_view( VkDevice device, TextureImage *textureImage );

void create_texture_sampler( VulkanDevice *vulkanDevice,
                             TextureImage *textureImage );

}  // namespace Image
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy