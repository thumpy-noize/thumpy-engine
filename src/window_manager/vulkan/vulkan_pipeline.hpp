/**
 * @file vulkan_pipeline.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Vulkan pipeline methods
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

#include "vulkan_device.hpp"
#include "vulkan_swap_chain.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

struct VulkanPipeline {
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
};

VulkanPipeline *create_graphics_pipeline( VulkanSwapChain *swapChain, VulkanDevice *vulkanDevice,
                                          VkDescriptorSetLayout descriptorSetLayout );

void destroy_graphics_pipeline( VkDevice vulkanDevice, VulkanPipeline *pipeline );

VkShaderModule create_shader_module( const std::vector<char> &code, VkDevice vulkanDevice );

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
