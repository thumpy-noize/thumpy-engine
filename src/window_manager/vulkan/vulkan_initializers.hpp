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
namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Initializer {

inline VkApplicationInfo application_info() {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Thumpy Engine Vulkan Window";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Thumpy Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;
  return appInfo;
}

inline VkCommandBufferAllocateInfo
command_buffer_allocate_info(VkCommandPool commandPool, uint32_t bufferCount) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = bufferCount;
  return allocInfo;
}

inline VkPipelineShaderStageCreateInfo
vert_shader_stage_info(VkShaderModule vertShaderModule) {
  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";
  return vertShaderStageInfo;
}

inline VkPipelineShaderStageCreateInfo
frag_shader_stage_info(VkShaderModule fragShaderModule) {
  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";
  return fragShaderStageInfo;
}

inline VkPipelineVertexInputStateCreateInfo vertex_input_info() {
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  return vertexInputInfo;
}

inline VkPipelineInputAssemblyStateCreateInfo input_assembly() {
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;
  return inputAssembly;
}

} // namespace Initializer
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
