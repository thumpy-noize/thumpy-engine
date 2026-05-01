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
namespace Image {

struct Texture {
  unsigned char* pixels = nullptr;
  int width = 0;
  int height = 0;
  int channels = 0;
  vk::DeviceSize imageSize = 0;
};

struct VulkanImage {
  vk::raii::Image image = nullptr;
  vk::raii::DeviceMemory imageMemory = nullptr;
  vk::raii::ImageView imageView = nullptr;
  //   VkImage image;
  //   VkDeviceMemory imageMemory;
  //   VkImageView imageView;

  //   void destroy( VkDevice device ) {
  //     vkDestroyImageView( device, imageView, nullptr );
  //     vkDestroyImage( device, image, nullptr );
  //     vkFreeMemory( device, imageMemory, nullptr );
  //   }
};

struct VulkanTextureImage : VulkanImage {
  vk::raii::Sampler textureSampler = nullptr;
  //   VkSampler sampler;
  //   uint32_t mipLevels;

  //   void destroy( VkDevice device ) {
  //     vkDestroySampler( device, sampler, nullptr );
  //     VulkanImage::destroy( device );
  //   }
};

void create_image( uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
                   vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
                   std::shared_ptr<VulkanDevice> vulkanDevice,
                   std::shared_ptr<VulkanImage> vulkanImage );
// void create_image( uint32_t width, uint32_t height, uint32_t mipLevels,
//                    VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
//                    VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
//                    VulkanImage *textureImage, VulkanDevice *vulkanDevice );

void create_texture_image( std::shared_ptr<VulkanDevice> vulkanDevice,
                           vk::raii::CommandPool& commandPool,
                           std::shared_ptr<Image::VulkanImage> vulkanImage, std::string filePath );
// void create_texture_image( VulkanDevice *vulkanDevice, VulkanTextureImage *textureImage,
//                            VkCommandPool commandPool, std::string filePath );

void transition_image_layout( vk::raii::Image& image, vk::ImageLayout oldLayout,
                              vk::ImageLayout newLayout, std::shared_ptr<VulkanDevice> vulkanDevice,
                              vk::raii::CommandPool& commandPool );
// void transition_image_layout( VkImage image, VkFormat format, VkImageLayout oldLayout,
//                               VkImageLayout newLayout, VulkanDevice *vulkanDevice,
//                               VkCommandPool commandPool, uint32_t mipLevels );

void copy_buffer_to_image( const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width,
                           uint32_t height, std::shared_ptr<VulkanDevice> vulkanDevice,
                           vk::raii::CommandPool& commandPool );
// void copy_buffer_to_image( VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
//                            VulkanDevice *vulkanDevice, VkCommandPool commandPool );

vk::raii::ImageView create_image_view( vk::raii::Image& image, vk::Format format,
                                       vk::ImageAspectFlags aspectFlags,
                                       std::shared_ptr<VulkanDevice> vulkanDevice );
// VkImageView create_image_view( VkDevice device, VkImage image, VkFormat format,
//                                VkImageAspectFlags aspectFlags, uint32_t mipLevels );

void create_texture_image_view( std::shared_ptr<Image::VulkanImage> vulkanImage,
                                std::shared_ptr<VulkanDevice> vulkanDevice );
// void create_texture_image_view( VkDevice device, VulkanTextureImage *textureImage );

void create_texture_sampler( std::shared_ptr<Image::VulkanTextureImage> vulkanTextureImage,
                             std::shared_ptr<VulkanDevice> vulkanDevice );
// void create_texture_sampler( VulkanDevice *vulkanDevice, VulkanTextureImage *textureImage );

void create_depth_resources( std::shared_ptr<Image::VulkanImage> depthBuffer,
                             std::shared_ptr<VulkanDevice> vulkanDevice,
                             vk::Extent2D& swapChainExtent );
// void create_depth_resources( VulkanImage *depthBuffer, VulkanDevice *vulkanDevice,
//                              VkExtent2D swapChainExtent );

vk::Format find_supported_format( const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
                                  vk::FormatFeatureFlags features,
                                  vk::raii::PhysicalDevice& physicalDevice );
// VkFormat find_supported_format( const std::vector<VkFormat> &candidates, VkImageTiling tiling,
//                                 VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice
//                                 );

vk::Format find_depth_format( vk::raii::PhysicalDevice& physicalDevice );
// VkFormat find_depth_format( VkPhysicalDevice physicalDevice );

bool has_stencil_component( vk::Format format );
// bool has_stencil_component( VkFormat format );

// void generate_mipmaps( VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t
// texHeight,
//                        uint32_t mipLevels, VulkanDevice *vulkanDevice, VkCommandPool
//                        commandPool
//                        );

// void create_color_resources( VulkanImage *msaaColorBuffer, VulkanDevice *vulkanDevice,
//                              VulkanSwapChain *swapChain );

}  // namespace Image
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy