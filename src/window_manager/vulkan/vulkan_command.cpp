/**
 * @file vulkan_command.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief vulkan_command cpp file
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vulkan_command.hpp"
#include "logger.hpp"
#include "logger_helper.hpp"
#include "vulkan/vulkan_helper.hpp"
#include "vulkan/vulkan_initializers.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_swap_chain.hpp"
#include <vulkan/vulkan_core.h>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

void create_command_pool(VulkanDevice *vulkanDevice,
                         VkCommandPool &commandPool) {
  QueueFamilyIndices queueFamilyIndices =
      vulkanDevice->find_queue_families(vulkanDevice->physicalDevice);

  VkCommandPoolCreateInfo poolInfo =
      Initializer::pool_info(queueFamilyIndices.graphicsFamily.value());

  if (vkCreateCommandPool(vulkanDevice->device, &poolInfo, nullptr,
                          &commandPool) != VK_SUCCESS) {
    Logger::log("Failed to create command pool!", Logger::CRITICAL);
  }
}

void create_command_buffer(std::vector<VkCommandBuffer> &commandBuffers,
                           VkCommandPool commandPool, VkDevice device,
                           int max_frames_in_flight) {
  commandBuffers.resize(max_frames_in_flight);
  VkCommandBufferAllocateInfo allocInfo =
      Initializer::command_buffer_allocate_info(
          commandPool, (uint32_t)commandBuffers.size());

  if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) !=
      VK_SUCCESS) {
    Logger::log("Failed to allocate command buffers!", Logger::CRITICAL);
  }
}

void record_command_buffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                           VulkanSwapChain *swapChain,
                           VulkanPipeline pipeline) {
  VkCommandBufferBeginInfo beginInfo = Initializer::command_buffer_begin_info();

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    Logger::log("Failed to begin recording command buffer!", Logger::CRITICAL);
  }

  VkRenderPassBeginInfo renderPassInfo = Initializer::render_pass_info(
      swapChain->renderPass, swapChain->swapChainFramebuffers[imageIndex],
      swapChain->extent);

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; // background color
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline.graphicsPipeline);

  VkViewport viewport =
      Initializer::viewport(static_cast<float>(swapChain->extent.height),
                            static_cast<float>(swapChain->extent.width));
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor = Initializer::scissor(swapChain->extent);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  vkCmdDraw(commandBuffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    Logger::log("Failed to record command buffer!", Logger::CRITICAL);
  }
}
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
