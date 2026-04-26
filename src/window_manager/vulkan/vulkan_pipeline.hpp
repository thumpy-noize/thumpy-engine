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
  vk::raii::PipelineLayout pipelineLayout = nullptr;
  vk::raii::Pipeline graphicsPipeline = nullptr;
};

std::shared_ptr<VulkanPipeline> create_graphics_pipeline(
    std::shared_ptr<VulkanDevice> vulkanDevice, std::shared_ptr<VulkanSwapChain> swapChain );

// void destroy_graphics_pipeline( VkDevice vulkanDevice, VulkanPipeline *pipeline );

[[nodiscard]] vk::raii::ShaderModule create_shader_module(
    const std::vector<char> &code, std::shared_ptr<VulkanDevice> vulkanDevice );

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
