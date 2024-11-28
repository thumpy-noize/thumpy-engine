/**
 * @file vulkan_render.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Vulkan drawing methods
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "vulkan_render.hpp"
#include "logger.hpp"
#include "logger_helper.hpp"
#include "vulkan/vulkan_device.hpp"
#include "vulkan/vulkan_initializers.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_swap_chain.hpp"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

VulkanRender::VulkanRender(int maxFramesInFlight, VulkanDevice *vulkanDevice,
                           VulkanSwapChain *swapChain,
                           std::vector<VkCommandBuffer> *commandBuffers,
                           VulkanPipeline pipeline) {
  set_context(maxFramesInFlight, vulkanDevice, swapChain, commandBuffers,
              pipeline);
  create_sync_objects();
}

void VulkanRender::destroy() {
  for (size_t i = 0; i < maxFramesInFlight_; i++) {
    vkDestroySemaphore(vulkanDevice_->device, renderFinishedSemaphores_[i],
                       nullptr);
    vkDestroySemaphore(vulkanDevice_->device, imageAvailableSemaphores_[i],
                       nullptr);
    vkDestroyFence(vulkanDevice_->device, inFlightFences_[i], nullptr);
  }
}

void VulkanRender::set_context(int maxFramesInFlight,
                               VulkanDevice *vulkanDevice,
                               VulkanSwapChain *swapChain,
                               std::vector<VkCommandBuffer> *commandBuffers,
                               VulkanPipeline pipeline) {
  maxFramesInFlight_ = maxFramesInFlight;
  vulkanDevice_ = vulkanDevice;
  swapChain_ = swapChain;
  commandBuffers_ = *commandBuffers;
  pipeline_ = pipeline;
}

void VulkanRender::create_sync_objects() {
  imageAvailableSemaphores_.resize(maxFramesInFlight_);
  renderFinishedSemaphores_.resize(maxFramesInFlight_);
  inFlightFences_.resize(maxFramesInFlight_);

  VkSemaphoreCreateInfo semaphoreInfo = Initializer::semaphore_info();

  VkFenceCreateInfo fenceInfo = Initializer::fence_info();

  for (size_t i = 0; i < maxFramesInFlight_; i++) {
    if (vkCreateSemaphore(vulkanDevice_->device, &semaphoreInfo, nullptr,
                          &imageAvailableSemaphores_[i]) != VK_SUCCESS ||
        vkCreateSemaphore(vulkanDevice_->device, &semaphoreInfo, nullptr,
                          &renderFinishedSemaphores_[i]) != VK_SUCCESS ||
        vkCreateFence(vulkanDevice_->device, &fenceInfo, nullptr,
                      &inFlightFences_[i]) != VK_SUCCESS) {

      throw std::runtime_error(
          "failed to create synchronization objects for a frame!");
    }
  }
}

void VulkanRender::draw_frame(VkBuffer vertexBuffer, uint32_t vertexCount) {

  vkWaitForFences(vulkanDevice_->device, 1, &inFlightFences_[currentFrame_],
                  VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      vulkanDevice_->device, swapChain_->swapChain, UINT64_MAX,
      imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    swapChain_->recreate_swap_chain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  vkResetFences(vulkanDevice_->device, 1, &inFlightFences_[currentFrame_]);

  vkResetCommandBuffer(commandBuffers_[currentFrame_],
                       /*VkCommandBufferResetFlagBits*/ 0);
  record_command_buffer(commandBuffers_[currentFrame_], imageIndex, swapChain_,
                        vertexBuffer, vertexCount);

  // vkAcquireNextImageKHR(vulkanDevice_->device, swapChain_->swapChain,
  //                       UINT64_MAX, imageAvailableSemaphores_[currentFrame_],
  //                       VK_NULL_HANDLE, &imageIndex);

  // submit command buffer
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores_[currentFrame_]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphores_[currentFrame_]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(vulkanDevice_->graphicsQueue, 1, &submitInfo,
                    inFlightFences_[currentFrame_]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swapChain_->swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr; // Optional

  result = vkQueuePresentKHR(vulkanDevice_->presentQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      framebufferResized_) {
    framebufferResized_ = false;
    swapChain_->recreate_swap_chain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }
}

void VulkanRender::record_command_buffer(VkCommandBuffer commandBuffer,
                                         uint32_t imageIndex,
                                         VulkanSwapChain *swapChain,
                                         VkBuffer vertexBuffer,
                                         uint32_t vertexCount) {
  VkCommandBufferBeginInfo beginInfo = Initializer::command_buffer_begin_info();

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
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
                    pipeline_.graphicsPipeline);

  VkBuffer vertexBuffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

  VkViewport viewport =
      Initializer::viewport(static_cast<float>(swapChain->extent.height),
                            static_cast<float>(swapChain->extent.width));
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor = Initializer::scissor(swapChain->extent);
  // VkRect2D scissor{};
  // scissor.offset = {0, 0};
  // scissor.extent = swapChain->extent;
  // vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
