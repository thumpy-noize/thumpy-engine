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

#include <sys/types.h>
#include <vulkan/vulkan_core.h>

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

#include <cstdint>
#include <stdexcept>
#include <string>

#include "logger.hpp"
#include "vulkan_buffers.hpp"
#include "vulkan_initializers.hpp"
#include "vulkan_swap_chain.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

namespace Image {

void create_image( uint32_t width, uint32_t height, uint32_t mipLevels,
                   vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling,
                   vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
                   std::shared_ptr<VulkanDevice> vulkanDevice,
                   std::shared_ptr<VulkanImage> vulkanImage ) {
  // Create image info
  vk::ImageCreateInfo imageInfo =
      Initializer::image_info( width, height, format, tiling, usage, mipLevels, numSamples );

  // Create image
  vulkanImage->image = vk::raii::Image( vulkanDevice->device, imageInfo );

  // Get memory requirements
  vk::MemoryRequirements memRequirements = vulkanImage->image.getMemoryRequirements();

  // Get memory allocation info
  vk::MemoryAllocateInfo allocInfo{
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = find_memory_type( vulkanDevice->physicalDevice,
                                           memRequirements.memoryTypeBits, properties ) };

  // Create image memory
  vulkanImage->imageMemory = vk::raii::DeviceMemory( vulkanDevice->device, allocInfo );

  // Bind memory
  vulkanImage->image.bindMemory( vulkanImage->imageMemory, 0 );
}

void create_texture_image( std::shared_ptr<VulkanDevice> vulkanDevice,
                           vk::raii::CommandPool& commandPool,
                           std::shared_ptr<Image::VulkanTextureImage> vulkanTextureImage,
                           std::string filePath ) {
  // Load texture
  std::shared_ptr<Texture> texture = load_texture( filePath );

  // Get mip levels
  vulkanTextureImage->mipLevels = static_cast<uint32_t>( std::floor(
                                      std::log2( std::max( texture->width, texture->height ) ) ) ) +
                                  1;

  // Create staging buffer
  std::shared_ptr<Buffer::Buffer> stagingBuffer = std::make_shared<Buffer::Buffer>();
  Buffer::create_buffer(
      texture->imageSize, vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
      stagingBuffer, vulkanDevice );

  // Map memory
  void* data = stagingBuffer->memory.mapMemory( 0, texture->imageSize );
  memcpy( data, texture->pixels, texture->imageSize );
  stagingBuffer->memory.unmapMemory();

  // Free pixel array
  free_texture( texture );

  // Create image
  create_image( texture->width, texture->height, vulkanTextureImage->mipLevels,
                vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
                    vk::ImageUsageFlagBits::eSampled,
                vk::MemoryPropertyFlagBits::eDeviceLocal, vulkanDevice, vulkanTextureImage );

  // Transistion layout
  transition_image_layout( vulkanTextureImage->image, vk::ImageLayout::eUndefined,
                           vk::ImageLayout::eTransferDstOptimal, vulkanTextureImage->mipLevels,
                           vulkanDevice, commandPool );

  // Copy buffer to image
  copy_buffer_to_image( stagingBuffer->buffer, vulkanTextureImage->image,
                        static_cast<uint32_t>( texture->width ),
                        static_cast<uint32_t>( texture->height ), vulkanDevice, commandPool );

  // Generate mip maps
  generate_mipmaps( vulkanTextureImage->image, vk::Format::eR8G8B8A8Srgb, texture->width,
                    texture->height, vulkanTextureImage->mipLevels, vulkanDevice, commandPool );
}

void transition_image_layout( vk::raii::Image& image, vk::ImageLayout oldLayout,
                              vk::ImageLayout newLayout, uint32_t mipLevels,
                              std::shared_ptr<VulkanDevice> vulkanDevice,
                              vk::raii::CommandPool& commandPool ) {
  // Begin command buffer
  std::unique_ptr<vk::raii::CommandBuffer> commandBuffer =
      Buffer::begin_single_time_commands( vulkanDevice, commandPool );

  // Create image barrier
  vk::ImageMemoryBarrier barrier =
      Initializer::image_memory_barrier( image, oldLayout, newLayout, mipLevels );

  // Create pipeline stage flags
  vk::PipelineStageFlags sourceStage;
  vk::PipelineStageFlags destinationStage;

  // Set flags for layout changes
  if ( oldLayout == vk::ImageLayout::eUndefined &&
       newLayout == vk::ImageLayout::eTransferDstOptimal ) {
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

    sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
    destinationStage = vk::PipelineStageFlagBits::eTransfer;

  } else if ( oldLayout == vk::ImageLayout::eTransferDstOptimal &&
              newLayout == vk::ImageLayout::eShaderReadOnlyOptimal ) {
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    sourceStage = vk::PipelineStageFlagBits::eTransfer;
    destinationStage = vk::PipelineStageFlagBits::eFragmentShader;

  } else {
    Logger::log( "Unsupported layout transition!", Logger::CRITICAL );
    throw std::invalid_argument( "unsupported layout transition!" );
  }

  // Set pipline barrier
  commandBuffer->pipelineBarrier( sourceStage, destinationStage, {}, {}, nullptr, barrier );

  // End command buffer
  Buffer::end_single_time_commands( vulkanDevice, *commandBuffer );
}

void copy_buffer_to_image( const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width,
                           uint32_t height, std::shared_ptr<VulkanDevice> vulkanDevice,
                           vk::raii::CommandPool& commandPool ) {
  // Begin command buffer
  std::unique_ptr<vk::raii::CommandBuffer> commandBuffer =
      Buffer::begin_single_time_commands( vulkanDevice, commandPool );

  // Set region
  vk::BufferImageCopy region{ .bufferOffset = 0,
                              .bufferRowLength = 0,
                              .bufferImageHeight = 0,
                              .imageSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1 },
                              .imageOffset = { 0, 0, 0 },
                              .imageExtent = { width, height, 1 } };

  // Copy buffer to image
  commandBuffer->copyBufferToImage( buffer, image, vk::ImageLayout::eTransferDstOptimal,
                                    { region } );

  // End command buffer
  Buffer::end_single_time_commands( vulkanDevice, *commandBuffer );
}

vk::raii::ImageView create_image_view( vk::raii::Image& image, vk::Format format,
                                       vk::ImageAspectFlags aspectFlags, uint32_t mipLevels,
                                       std::shared_ptr<VulkanDevice> vulkanDevice ) {
  // Create image view info
  vk::ImageViewCreateInfo viewInfo{ .image = image,
                                    .viewType = vk::ImageViewType::e2D,
                                    .format = format,
                                    .subresourceRange = { aspectFlags, 0, mipLevels, 0, 1 } };

  // Create image view
  return vk::raii::ImageView( vulkanDevice->device, viewInfo );
}

void create_texture_image_view( std::shared_ptr<Image::VulkanTextureImage> vulkanTextureImage,
                                std::shared_ptr<VulkanDevice> vulkanDevice ) {
  // Create image view
  vulkanTextureImage->imageView = create_image_view(
      vulkanTextureImage->image, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor,
      vulkanTextureImage->mipLevels, vulkanDevice );
}

void create_texture_sampler( std::shared_ptr<Image::VulkanTextureImage> vulkanTextureImage,
                             std::shared_ptr<VulkanDevice> vulkanDevice ) {
  // Get device properties
  vk::PhysicalDeviceProperties properties = vulkanDevice->physicalDevice.getProperties();

  // Create sampler info // TODO: we likely want to use repeat instead of clamp to edge
  vk::SamplerCreateInfo samplerInfo{ .magFilter = vk::Filter::eLinear,
                                     .minFilter = vk::Filter::eLinear,
                                     .mipmapMode = vk::SamplerMipmapMode::eLinear,
                                     .addressModeU = vk::SamplerAddressMode::eRepeat,
                                     .addressModeV = vk::SamplerAddressMode::eRepeat,
                                     .addressModeW = vk::SamplerAddressMode::eRepeat,
                                     .mipLodBias = 0.0f,
                                     .anisotropyEnable = vk::True,
                                     .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
                                     .compareEnable = vk::False,
                                     .compareOp = vk::CompareOp::eAlways,
                                     .minLod = 0.0f,
                                     .maxLod = vk::LodClampNone };

  // Create texture sampler
  vulkanTextureImage->textureSampler = vk::raii::Sampler( vulkanDevice->device, samplerInfo );
}

void create_depth_resources( std::shared_ptr<Image::VulkanImage> depthBuffer,
                             std::shared_ptr<VulkanDevice> vulkanDevice,
                             vk::Extent2D& swapChainExtent ) {
  // Find depth format
  vk::Format depthFormat = find_depth_format( vulkanDevice->physicalDevice );

  // Create depth image
  create_image( swapChainExtent.width, swapChainExtent.height, 1, vulkanDevice->msaaSamples,
                depthFormat, vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal, vulkanDevice, depthBuffer );

  // Create depth image view
  depthBuffer->imageView = create_image_view( depthBuffer->image, depthFormat,
                                              vk::ImageAspectFlagBits::eDepth, 1, vulkanDevice );
}

vk::Format find_supported_format( const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
                                  vk::FormatFeatureFlags features,
                                  vk::raii::PhysicalDevice& physicalDevice ) {
  // Find format candidates
  auto formatIt = std::ranges::find_if( candidates, [&]( auto const format ) {
    // Get properties from device
    vk::FormatProperties properties = physicalDevice.getFormatProperties( format );

    return ( ( ( tiling == vk::ImageTiling::eLinear ) &&
               ( ( properties.linearTilingFeatures & features ) == features ) ) ||
             ( ( tiling == vk::ImageTiling::eOptimal ) &&
               ( ( properties.optimalTilingFeatures & features ) == features ) ) );
  } );

  // Failed to find supported format
  if ( formatIt == candidates.end() ) {
    Logger::log( "Failed to find supported format!", Logger::CRITICAL );
    throw std::runtime_error( "failed to find supported format!" );
  }

  // return format iterator
  return *formatIt;
}

vk::Format find_depth_format( vk::raii::PhysicalDevice& physicalDevice ) {
  // Find supported format
  return find_supported_format(
      { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
      vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment,
      physicalDevice );
}

bool has_stencil_component( vk::Format format ) {
  // Check for stencil component
  return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void generate_mipmaps( vk::raii::Image& image, vk::Format imageFormat, int32_t texWidth,
                       int32_t texHeight, uint32_t mipLevels,
                       std::shared_ptr<VulkanDevice> vulkanDevice,
                       vk::raii::CommandPool& commandPool ) {
  // Check if image format supports linear blit-ing
  vk::FormatProperties formatProperties =
      vulkanDevice->physicalDevice.getFormatProperties( imageFormat );

  if ( !( formatProperties.optimalTilingFeatures &
          vk::FormatFeatureFlagBits::eSampledImageFilterLinear ) ) {
    Logger::log( "Texture image format does not support linear blitting!", Logger::ERROR_LOG );
    throw std::runtime_error( "texture image format does not support linear blitting!" );
  }

  // Start command buffer
  std::unique_ptr<vk::raii::CommandBuffer> commandBuffer =
      Buffer::begin_single_time_commands( vulkanDevice, commandPool );

  // Create barrier info
  vk::ImageMemoryBarrier barrier = { .srcAccessMask = vk::AccessFlagBits::eTransferWrite,
                                     .dstAccessMask = vk::AccessFlagBits::eTransferRead,
                                     .oldLayout = vk::ImageLayout::eTransferDstOptimal,
                                     .newLayout = vk::ImageLayout::eTransferSrcOptimal,
                                     .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
                                     .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
                                     .image = image };

  barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.subresourceRange.levelCount = 1;

  // Set width / height
  int32_t mipWidth = texWidth;
  int32_t mipHeight = texHeight;

  // For each mip level
  for ( uint32_t i = 1; i < mipLevels; i++ ) {
    // Create barrier info
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

    // Set pipeline barrier
    commandBuffer->pipelineBarrier( vk::PipelineStageFlagBits::eTransfer,
                                    vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, barrier );

    // Create offsets
    vk::ArrayWrapper1D<vk::Offset3D, 2> offsets, dstOffsets;
    offsets[0] = vk::Offset3D( 0, 0, 0 );
    offsets[1] = vk::Offset3D( mipWidth, mipHeight, 1 );
    dstOffsets[0] = vk::Offset3D( 0, 0, 0 );
    dstOffsets[1] =
        vk::Offset3D( mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 );

    // Create image blit
    vk::ImageBlit blit = { .srcSubresource = {},
                           .srcOffsets = offsets,
                           .dstSubresource = {},
                           .dstOffsets = dstOffsets };
    blit.srcSubresource =
        vk::ImageSubresourceLayers( vk::ImageAspectFlagBits::eColor, i - 1, 0, 1 );
    blit.dstSubresource = vk::ImageSubresourceLayers( vk::ImageAspectFlagBits::eColor, i, 0, 1 );

    // Record blit image
    commandBuffer->blitImage( image, vk::ImageLayout::eTransferSrcOptimal, image,
                              vk::ImageLayout::eTransferDstOptimal, { blit }, vk::Filter::eLinear );

    barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    commandBuffer->pipelineBarrier( vk::PipelineStageFlagBits::eTransfer,
                                    vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {},
                                    barrier );

    // Divide image height / width by 2, and repeat
    if ( mipWidth > 1 ) mipWidth /= 2;
    if ( mipHeight > 1 ) mipHeight /= 2;
  }

  // Barrier info
  barrier.subresourceRange.baseMipLevel = mipLevels - 1;
  barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
  barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
  barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
  barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

  // Set pipeline barrier
  commandBuffer->pipelineBarrier( vk::PipelineStageFlagBits::eTransfer,
                                  vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrier );

  // End command buffer
  Buffer::end_single_time_commands( vulkanDevice, *commandBuffer );
}

void create_color_resources( std::shared_ptr<VulkanImage> vulkanImage,
                             std::shared_ptr<VulkanDevice> vulkanDevice,
                             std::shared_ptr<VulkanSwapChain> swapChain ) {
  // Get color format
  vk::Format colorFormat = swapChain->swapChainSurfaceFormat.format;

  // Create msaa image
  create_image(
      swapChain->swapChainExtent.width, swapChain->swapChainExtent.height, 1,
      vulkanDevice->msaaSamples, colorFormat, vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
      vk::MemoryPropertyFlagBits::eDeviceLocal, vulkanDevice, vulkanImage );

  // Create msaa image view
  vulkanImage->imageView = create_image_view( vulkanImage->image, colorFormat,
                                              vk::ImageAspectFlagBits::eColor, 1, vulkanDevice );
}

}  // namespace Image
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy