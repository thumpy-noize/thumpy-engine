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

#include <vulkan/vulkan_core.h>

#include <cstdint>

#include "vulkan/vulkan_swap_chain.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Image {

void create_image( uint32_t width, uint32_t height, uint32_t mipLevels,
                   VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
                   VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                   VulkanImage *textureImage, VulkanDevice *vulkanDevice );

void create_texture_image( VulkanDevice *vulkanDevice, VulkanTextureImage *textureImage,
                           VkCommandPool commandPool, std::string filePath );

void transition_image_layout( VkImage image, VkFormat format, VkImageLayout oldLayout,
                              VkImageLayout newLayout, VulkanDevice *vulkanDevice,
                              VkCommandPool commandPool, uint32_t mipLevels );

void copy_buffer_to_image( VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
                           VulkanDevice *vulkanDevice, VkCommandPool commandPool );

VkImageView create_image_view( VkDevice device, VkImage image, VkFormat format,
                               VkImageAspectFlags aspectFlags, uint32_t mipLevels );

void create_texture_image_view( VkDevice device, VulkanTextureImage *textureImage );

void create_texture_sampler( VulkanDevice *vulkanDevice, VulkanTextureImage *textureImage );

void create_depth_resources( VulkanImage *depthBuffer, VulkanDevice *vulkanDevice,
                             VkExtent2D swapChainExtent );

VkFormat find_supported_format( const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice );

VkFormat find_depth_format( VkPhysicalDevice physicalDevice );

bool has_stencil_component( VkFormat format );

void generate_mipmaps( VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight,
                       uint32_t mipLevels, VulkanDevice *vulkanDevice, VkCommandPool commandPool );

void create_color_resources( VulkanImage *msaaColorBuffer, VulkanSwapChain *swapChain,
                             VulkanDevice *vulkanDevice );

}  // namespace Image
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy