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

#include <vulkan/vulkan_core.h>

#include <chrono>
#include <cstdint>
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <string>

#include "logger.hpp"
#include "logger_helper.hpp"
#include "vulkan/vulkan_device.hpp"
#include "vulkan/vulkan_initializers.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_swap_chain.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

VulkanRender::VulkanRender( int maxFramesInFlight, VulkanDevice *vulkanDevice,
                            VulkanSwapChain *swapChain,
                            std::vector<VkCommandBuffer> *commandBuffers,
                            VulkanPipeline *pipeline ) {
  maxFramesInFlight_ = maxFramesInFlight;
  vulkanDevice_ = vulkanDevice;
  swapChain_ = swapChain;
  commandBuffers_ = *commandBuffers;
  pipeline_ = pipeline;

  create_sync_objects();
}

void VulkanRender::destroy() {
  for ( size_t i = 0; i < maxFramesInFlight_; i++ ) {
    vkDestroySemaphore( vulkanDevice_->device, renderFinishedSemaphores_[i],
                        nullptr );
    vkDestroySemaphore( vulkanDevice_->device, imageAvailableSemaphores_[i],
                        nullptr );
    vkDestroyFence( vulkanDevice_->device, inFlightFences_[i], nullptr );
  }
}

void VulkanRender::create_sync_objects() {
  imageAvailableSemaphores_.resize( maxFramesInFlight_ );
  renderFinishedSemaphores_.resize( maxFramesInFlight_ );
  inFlightFences_.resize( maxFramesInFlight_ );

  VkSemaphoreCreateInfo semaphoreInfo = Initializer::semaphore_info();

  VkFenceCreateInfo fenceInfo = Initializer::fence_info();

  for ( size_t i = 0; i < maxFramesInFlight_; i++ ) {
    if ( vkCreateSemaphore( vulkanDevice_->device, &semaphoreInfo, nullptr,
                            &imageAvailableSemaphores_[i] ) != VK_SUCCESS ||
         vkCreateSemaphore( vulkanDevice_->device, &semaphoreInfo, nullptr,
                            &renderFinishedSemaphores_[i] ) != VK_SUCCESS ||
         vkCreateFence( vulkanDevice_->device, &fenceInfo, nullptr,
                        &inFlightFences_[i] ) != VK_SUCCESS ) {
      Logger::log( "Failed to create synchronization objects for a frame!",
                   Logger::CRITICAL );
    }
  }
}

void VulkanRender::draw_frame( VkBuffer vertexBuffer, uint32_t vertexCount,
                               VkBuffer indexBuffer, uint32_t indexCount,
                               std::vector<void *> uniformBuffersMapped,
                               std::vector<VkDescriptorSet> descriptorSets ) {
  vkWaitForFences( vulkanDevice_->device, 1, &inFlightFences_[currentFrame_],
                   VK_TRUE, UINT64_MAX );

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      vulkanDevice_->device, swapChain_->swapChain, UINT64_MAX,
      imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &imageIndex );

  if ( result == VK_ERROR_OUT_OF_DATE_KHR ) {
    swapChain_->recreate_swap_chain();
    return;
  } else if ( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR ) {
    Logger::log( "Failed to acquire swap chain image!", Logger::CRITICAL );
  }

  update_uniform_buffer( currentFrame_, uniformBuffersMapped );

  vkResetFences( vulkanDevice_->device, 1, &inFlightFences_[currentFrame_] );

  vkResetCommandBuffer( commandBuffers_[currentFrame_],
                        /*VkCommandBufferResetFlagBits*/ 0 );
  record_command_buffer( commandBuffers_[currentFrame_], imageIndex, swapChain_,
                         vertexBuffer, vertexCount, indexBuffer, indexCount,
                         descriptorSets );

  // vkAcquireNextImageKHR(vulkanDevice_->device, swapChain_->swapChain,
  //                       UINT64_MAX, imageAvailableSemaphores_[currentFrame_],
  //                       VK_NULL_HANDLE, &imageIndex);

  // submit command buffer
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = { imageAvailableSemaphores_[currentFrame_] };
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];

  VkSemaphore signalSemaphores[] = { renderFinishedSemaphores_[currentFrame_] };
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if ( vkQueueSubmit( vulkanDevice_->graphicsQueue, 1, &submitInfo,
                      inFlightFences_[currentFrame_] ) != VK_SUCCESS ) {
    Logger::log( "Failed to submit draw command buffer!", Logger::CRITICAL );
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = { swapChain_->swapChain };
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;  // Optional

  result = vkQueuePresentKHR( vulkanDevice_->presentQueue, &presentInfo );

  if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
       framebufferResized_ ) {
    framebufferResized_ = false;
    swapChain_->recreate_swap_chain();
  } else if ( result != VK_SUCCESS ) {
    Logger::log( "Failed to present swap chain image!", Logger::CRITICAL );
  }
}

void VulkanRender::record_command_buffer(
    VkCommandBuffer commandBuffer, uint32_t imageIndex,
    VulkanSwapChain *swapChain, VkBuffer vertexBuffer, uint32_t vertexCount,
    VkBuffer indexBuffer, uint32_t indexCount,
    std::vector<VkDescriptorSet> descriptorSets ) {
  VkCommandBufferBeginInfo beginInfo = Initializer::command_buffer_begin_info();

  if ( vkBeginCommandBuffer( commandBuffer, &beginInfo ) != VK_SUCCESS ) {
    Logger::log( "Failed to begin recording command buffer!",
                 Logger::CRITICAL );
  }

  VkRenderPassBeginInfo renderPassInfo = Initializer::render_pass_info(
      swapChain->renderPass, swapChain->swapChainFramebuffers[imageIndex],
      swapChain->extent );

  VkClearValue clearColor = {
      { { 0.0f, 0.0f, 0.0f, 1.0f } } };  // background color
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass( commandBuffer, &renderPassInfo,
                        VK_SUBPASS_CONTENTS_INLINE );

  vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                     pipeline_->graphicsPipeline );

  VkViewport viewport =
      Initializer::viewport( static_cast<float>( swapChain->extent.height ),
                             static_cast<float>( swapChain->extent.width ) );
  vkCmdSetViewport( commandBuffer, 0, 1, &viewport );

  VkRect2D scissor = Initializer::scissor( swapChain->extent );
  // VkRect2D scissor{};
  // scissor.offset = {0, 0};
  // scissor.extent = swapChain->extent;
  vkCmdSetScissor( commandBuffer, 0, 1, &scissor );

  VkBuffer vertexBuffers[] = { vertexBuffer };
  VkDeviceSize offsets[] = { 0 };
  vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );

  vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );

  // vkCmdDraw( commandBuffer, vertexCount, 1, 0, 0 );

  vkCmdBindIndexBuffer( commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16 );

  vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                           pipeline_->pipelineLayout, 0, 1,
                           &descriptorSets[currentFrame_], 0, nullptr );

  vkCmdDrawIndexed( commandBuffer, indexCount, 1, 0, 0, 0 );
  vkCmdEndRenderPass( commandBuffer );

  if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS ) {
    Logger::log( "Failed to record command buffer!", Logger::CRITICAL );
  }
}

void VulkanRender::update_uniform_buffer(
    uint32_t currentImage, std::vector<void *> uniformBuffersMapped ) {
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float run_time = std::chrono::duration<float, std::chrono::seconds::period>(
                       currentTime - startTime )
                       .count();
  Logger::log( "Run time: " + std::to_string( run_time ), Logger::DEBUG );

  UniformBufferObject ubo{};
  ubo.model = glm::rotate( glm::mat4( 1.0f ), run_time * glm::radians( 90.0f ),
                           glm::vec3( 0.0f, 0.0f, 1.0f ) );
  ubo.view =
      glm::lookAt( glm::vec3( 2.0f, 2.0f, 2.0f ), glm::vec3( 0.0f, 0.0f, 0.0f ),
                   glm::vec3( 0.0f, 0.0f, 1.0f ) );
  ubo.proj = glm::perspective(
      glm::radians( 45.0f ),
      swapChain_->extent.width / (float)swapChain_->extent.height, 0.1f,
      10.0f );
  ubo.proj[1][1] *= -1;
  memcpy( uniformBuffersMapped[currentImage], &ubo, sizeof( ubo ) );
}

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
