/**
 * @file vulkan_initializers.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Initializers for Vulkan structures used by examples
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include "logger.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Initializer {

/**
 * @brief Creates application info.
 *
 * @return constexpr vk::ApplicationInfo
 */
inline constexpr vk::ApplicationInfo application_info() {
  constexpr vk::ApplicationInfo appInfo{
      .pApplicationName = "Thumpy Engine Editor",  // TODO: Convert to variable
      .applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
      .pEngineName = "Thumpy Engine",  // TODO: Convert to variable
      .engineVersion = VK_MAKE_VERSION( 1, 0, 0 ),
      .apiVersion = vk::ApiVersion14 };

  return appInfo;
}

inline vk::PipelineRasterizationStateCreateInfo rasterizer() {
  vk::PipelineRasterizationStateCreateInfo rasterizer{
      .depthClampEnable = vk::False,
      .rasterizerDiscardEnable = vk::False,
      .polygonMode = vk::PolygonMode::eFill,
      .cullMode = vk::CullModeFlagBits::eBack,
      .frontFace = vk::FrontFace::eCounterClockwise,
      .depthBiasEnable = vk::False,
      .lineWidth = 1.0f };

  return rasterizer;
}

inline vk::PipelineMultisampleStateCreateInfo multisampling( vk::SampleCountFlagBits msaaSamples ) {
  vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = msaaSamples,
                                                        .sampleShadingEnable = vk::False };

  return multisampling;
}

inline vk::PipelineColorBlendAttachmentState color_blend_attachment() {
  vk::PipelineColorBlendAttachmentState colorBlendAttachment{
      .blendEnable = vk::False,
      .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };

  return colorBlendAttachment;
}

inline vk::PipelineLayoutCreateInfo pipeline_layout_info(
    vk::raii::DescriptorSetLayout &descriptorSetLayout ) {
  vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
      .setLayoutCount = 1, .pSetLayouts = &*descriptorSetLayout, .pushConstantRangeCount = 0 };

  return pipelineLayoutInfo;
}

inline vk::ShaderModuleCreateInfo shader_module_create_info( const std::vector<char> &code ) {
  vk::ShaderModuleCreateInfo createInfo{
      .codeSize = code.size() * sizeof( char ),
      .pCode = reinterpret_cast<const uint32_t *>( code.data() ) };

  return createInfo;
}

inline vk::CommandPoolCreateInfo pool_info( uint32_t queueIndex ) {
  vk::CommandPoolCreateInfo poolInfo{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                      .queueFamilyIndex = queueIndex };

  return poolInfo;
}

inline vk::CommandBufferAllocateInfo command_buffer_allocate_info(
    vk::raii::CommandPool &commandPool, uint32_t bufferCount ) {
  vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool,
                                           .level = vk::CommandBufferLevel::ePrimary,
                                           .commandBufferCount = bufferCount };

  return allocInfo;
}

inline vk::ImageCreateInfo image_info( uint32_t width, uint32_t height, vk::Format format,
                                       vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                                       uint32_t mipLevels, vk::SampleCountFlagBits numSamples ) {
  vk::ImageCreateInfo imageInfo{ .imageType = vk::ImageType::e2D,
                                 .format = format,
                                 .extent = { width, height, 1 },
                                 .mipLevels = mipLevels,
                                 .arrayLayers = 1,
                                 .samples = numSamples,
                                 .tiling = tiling,
                                 .usage = usage,
                                 .sharingMode = vk::SharingMode::eExclusive };

  return imageInfo;
}

// Note: This is not currently use when creating mipmaps
inline vk::ImageMemoryBarrier image_memory_barrier( vk::raii::Image &image,
                                                    vk::ImageLayout oldLayout,
                                                    vk::ImageLayout newLayout,
                                                    uint32_t mipLevels ) {
  vk::ImageMemoryBarrier barrier{
      .oldLayout = oldLayout,
      .newLayout = newLayout,
      .image = image,
      .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, mipLevels, 0, 1 } };

  return barrier;
}

// ######################
// ##### Deprecated #####
// ######################

// We don't currently need this
// inline VkPipelineShaderStageCreateInfo vert_shader_stage_info( VkShaderModule vertShaderModule )
// {
//   VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
//   vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//   vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//   vertShaderStageInfo.module = vertShaderModule;
//   vertShaderStageInfo.pName = "main";
//   return vertShaderStageInfo;
// }

// We don't currently need this
// inline VkPipelineShaderStageCreateInfo frag_shader_stage_info( VkShaderModule fragShaderModule )
// {
//   VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
//   fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//   fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//   fragShaderStageInfo.module = fragShaderModule;
//   fragShaderStageInfo.pName = "main";
//   return fragShaderStageInfo;
// }

/**
 * @brief deprecated
 *
 * @return VkPipelineVertexInputStateCreateInfo
 */
// inline VkPipelineVertexInputStateCreateInfo vertex_input_info() {
//   VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//   vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//   Logger::log( "vertex_input_info is deprecated", Logger::WARNING );
//   return vertexInputInfo;
// }

// This feels unnecessary
// inline VkPipelineInputAssemblyStateCreateInfo input_assembly() {
//   VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
//   inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//   inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//   inputAssembly.primitiveRestartEnable = VK_FALSE;
//   return inputAssembly;
// }

// This feels unnecessary
// inline VkViewport viewport( float hight, float width ) {
//   VkViewport viewport{};
//   viewport.x = 0.0f;
//   viewport.y = 0.0f;
//   viewport.height = hight;
//   viewport.width = width;
//   viewport.minDepth = 0.0f;
//   viewport.maxDepth = 1.0f;
//   return viewport;
// }

// This feels unnecessary
// inline VkRect2D scissor( VkExtent2D extent ) {
//   VkRect2D scissor{};
//   scissor.offset = { 0, 0 };
//   scissor.extent = extent;
//   return scissor;
// }

// This feels unnecessary
// inline VkPipelineViewportStateCreateInfo viewport_state( VkViewport &viewport, VkRect2D &scissor
// ) {
//   VkPipelineViewportStateCreateInfo viewportState{};
//   viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//   viewportState.viewportCount = 1;
//   viewportState.pViewports = &viewport;
//   viewportState.scissorCount = 1;
//   viewportState.pScissors = &scissor;
//   return viewportState;
// }

// RAII implementation is not currently using frame buffers
// inline VkFramebufferCreateInfo framebuffer_info( VkRenderPass renderPass, VkExtent2D extent,
//                                                  std::array<VkImageView, 3> &attachments ) {
//   VkFramebufferCreateInfo framebufferInfo{};
//   framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//   framebufferInfo.renderPass = renderPass;
//   framebufferInfo.attachmentCount = static_cast<uint32_t>( attachments.size() );
//   framebufferInfo.pAttachments = attachments.data();
//   framebufferInfo.width = extent.width;
//   framebufferInfo.height = extent.height;
//   framebufferInfo.layers = 1;
//   return framebufferInfo;
// }

// This feels unnecessary
// inline VkCommandBufferBeginInfo command_buffer_begin_info() {
//   VkCommandBufferBeginInfo beginInfo{};
//   beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//   beginInfo.flags = 0;                   // Optional
//   beginInfo.pInheritanceInfo = nullptr;  // Optional
//   return beginInfo;
// }

// RAII implementation is not currently using render pass
// inline VkRenderPassBeginInfo render_pass_info( VkRenderPass renderPass, VkFramebuffer
// frameBuffer,
//                                                VkExtent2D extent ) {
//   VkRenderPassBeginInfo renderPassInfo{};
//   renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//   renderPassInfo.renderPass = renderPass;
//   renderPassInfo.framebuffer = frameBuffer;
//   renderPassInfo.renderArea.offset = { 0, 0 };
//   renderPassInfo.renderArea.extent = extent;
//   return renderPassInfo;
// }

// This feels unnecessary
// inline VkSemaphoreCreateInfo semaphore_info() {
//   VkSemaphoreCreateInfo semaphoreInfo{};
//   semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//   return semaphoreInfo;
// }

// This feels unnecessary
// inline VkFenceCreateInfo fence_info() {
//   VkFenceCreateInfo fenceInfo{};
//   fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//   fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//   return fenceInfo;
// }

}  // namespace Initializer
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
