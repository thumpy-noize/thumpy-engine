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

#define GLM_FORCE_DEPTH_xZERO_TO_ONE

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// #include "vulkan/vulkan_swap_chain.hpp"
#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

class VulkanSwapChain;

namespace Image {

// struct Texture {
//   unsigned char* pixels = nullptr;
//   int width = 0;
//   int height = 0;
//   int channels = 0;
//   vk::DeviceSize imageSize = 0;
// };

struct VulkanImage {
  vk::raii::Image image = nullptr;
  vk::raii::DeviceMemory imageMemory = nullptr;
  vk::raii::ImageView imageView = nullptr;
};

struct VulkanTextureImage : VulkanImage {
  vk::raii::Sampler textureSampler = nullptr;
  uint32_t mipLevels = 0;
};

void create_image( uint32_t width, uint32_t height, uint32_t mipLevels,
                   vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling,
                   vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
                   std::shared_ptr<VulkanDevice> vulkanDevice,
                   std::shared_ptr<VulkanImage> vulkanImage );

void create_texture_image( std::shared_ptr<VulkanDevice> vulkanDevice,
                           vk::raii::CommandPool& commandPool,
                           std::shared_ptr<Image::VulkanTextureImage> vulkanTextureImage,
                           std::string filePath );

void transition_image_layout( vk::raii::Image& image, vk::ImageLayout oldLayout,
                              vk::ImageLayout newLayout, uint32_t mipLevels,
                              std::shared_ptr<VulkanDevice> vulkanDevice,
                              vk::raii::CommandPool& commandPool );

void copy_buffer_to_image( const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width,
                           uint32_t height, std::shared_ptr<VulkanDevice> vulkanDevice,
                           vk::raii::CommandPool& commandPool );

vk::raii::ImageView create_image_view( vk::raii::Image& image, vk::Format format,
                                       vk::ImageAspectFlags aspectFlags, uint32_t mipLevels,
                                       std::shared_ptr<VulkanDevice> vulkanDevice );

void create_texture_image_view( std::shared_ptr<Image::VulkanTextureImage> vulkanTextureImage,
                                std::shared_ptr<VulkanDevice> vulkanDevice );

void create_texture_sampler( std::shared_ptr<Image::VulkanTextureImage> vulkanTextureImage,
                             std::shared_ptr<VulkanDevice> vulkanDevice );

void create_depth_resources( std::shared_ptr<Image::VulkanImage> depthBuffer,
                             std::shared_ptr<VulkanDevice> vulkanDevice,
                             vk::Extent2D& swapChainExtent );

vk::Format find_supported_format( const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
                                  vk::FormatFeatureFlags features,
                                  vk::raii::PhysicalDevice& physicalDevice );

vk::Format find_depth_format( vk::raii::PhysicalDevice& physicalDevice );

bool has_stencil_component( vk::Format format );

void generate_mipmaps( vk::raii::Image& image, vk::Format imageFormat, int32_t texWidth,
                       int32_t texHeight, uint32_t mipLevels,
                       std::shared_ptr<VulkanDevice> vulkanDevice,
                       vk::raii::CommandPool& commandPool );

void create_color_resources( std::shared_ptr<VulkanImage> vulkanImage,
                             std::shared_ptr<VulkanDevice> vulkanDevice,
                             std::shared_ptr<VulkanSwapChain> swapChain );

}  // namespace Image
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy