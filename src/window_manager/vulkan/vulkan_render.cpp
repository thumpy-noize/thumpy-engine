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
#include "vulkan_initializers.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

VulkanRender::VulkanRender( std::shared_ptr<VulkanDevice> vulkanDevice,
                            std::shared_ptr<VulkanSwapChain> swapChain,
                            std::shared_ptr<VulkanPipeline> pipeline,
                            std::shared_ptr<Construct::CommandPool> commandPool,
                            int maxFramesInFlight ) {
  Logger::log( "Constructing render...", Logger::DEBUG );

  maxFramesInFlight_ = maxFramesInFlight;

  vulkanDevice_ = vulkanDevice;
  swapChain_ = swapChain;
  pipeline_ = pipeline;
  commandPool_ = commandPool;

  create_sync_objects();

  // maxFramesInFlight_ = maxFramesInFlight;
  // vulkanDevice_ = vulkanDevice;
  // swapChain_ = swapChain;
  // commandBuffers_ = *commandBuffers;
  // pipeline_ = pipeline;

  // create_sync_objects();
}

void VulkanRender::record_command_buffer( uint32_t imageIndex,
                                          std::shared_ptr<Buffer::Buffer> vertexBuffer,
                                          uint32_t vertexCount,
                                          std::shared_ptr<Buffer::Buffer> indexBuffer,
                                          uint16_t indexCount ) {
  // Get current buffer
  auto &commandBuffer = commandPool_->buffers[frameIndex_];

  // Begin command buffer
  commandBuffer.begin( {} );

  // Transistion swap chain
  transition_image_layout( imageIndex, vk::ImageLayout::eUndefined,
                           vk::ImageLayout::eColorAttachmentOptimal,
                           {},  // srcAccessMask (no need to wait for previous operations)
                           vk::AccessFlagBits2::eColorAttachmentWrite,          // dstAccessMask
                           vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // srcStage
                           vk::PipelineStageFlagBits2::eColorAttachmentOutput   // dstStage
  );

  // Set clear color
  vk::ClearValue clearColor = vk::ClearColorValue( 0.0f, 0.0f, 0.0f, 1.0f );

  // Create attachment info
  vk::RenderingAttachmentInfo attachmentInfo = {
      .imageView = swapChain_.lock()->swapChainImageViews[imageIndex],
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = clearColor };

  // Create rendering info
  vk::RenderingInfo renderingInfo = {
      .renderArea = { .offset = { 0, 0 }, .extent = swapChain_.lock()->swapChainExtent },
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &attachmentInfo };

  // Begin rendering
  commandBuffer.beginRendering( renderingInfo );

  // Bind pipeline
  commandBuffer.bindPipeline( vk::PipelineBindPoint::eGraphics,
                              *pipeline_.lock()->graphicsPipeline );

  // set viewport / scissor
  commandBuffer.setViewport(
      0,
      vk::Viewport( 0.0f, 0.0f, static_cast<float>( swapChain_.lock()->swapChainExtent.width ),
                    static_cast<float>( swapChain_.lock()->swapChainExtent.height ), 0.0f, 1.0f ) );
  commandBuffer.setScissor(
      0, vk::Rect2D( vk::Offset2D( 0, 0 ), swapChain_.lock()->swapChainExtent ) );

  // Bind vertex buffer
  commandBuffer.bindVertexBuffers( 0, *vertexBuffer->buffer, { 0 } );

  // Bind Index buffer
  commandBuffer.bindIndexBuffer( *indexBuffer->buffer, 0, vk::IndexType::eUint16 );

  // Draw buffer
  commandBuffer.drawIndexed( indexCount, 1, 0, 0, 0 );

  // End rendering
  commandBuffer.endRendering();

  // Transition to image
  transition_image_layout( imageIndex, vk::ImageLayout::eColorAttachmentOptimal,
                           vk::ImageLayout::ePresentSrcKHR,
                           vk::AccessFlagBits2::eColorAttachmentWrite,          // srcAccessMask
                           {},                                                  // dstAccessMask
                           vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // srcStage
                           vk::PipelineStageFlagBits2::eBottomOfPipe            // dstStage
  );

  // End command buffer
  commandBuffer.end();
}

void VulkanRender::transition_image_layout( uint32_t imageIndex, vk::ImageLayout old_layout,
                                            vk::ImageLayout new_layout,
                                            vk::AccessFlags2 src_access_mask,
                                            vk::AccessFlags2 dst_access_mask,
                                            vk::PipelineStageFlags2 src_stage_mask,
                                            vk::PipelineStageFlags2 dst_stage_mask ) {
  vk::ImageMemoryBarrier2 barrier = {
      .srcStageMask = src_stage_mask,
      .srcAccessMask = src_access_mask,
      .dstStageMask = dst_stage_mask,
      .dstAccessMask = dst_access_mask,
      .oldLayout = old_layout,
      .newLayout = new_layout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = swapChain_.lock()->swapChainImages[imageIndex],
      .subresourceRange = { .aspectMask = vk::ImageAspectFlagBits::eColor,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1 } };
  vk::DependencyInfo dependency_info = {
      .dependencyFlags = {}, .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrier };
  commandPool_->buffers[frameIndex_].pipelineBarrier2( dependency_info );
}

void VulkanRender::draw_frame( bool &framebufferResized,
                               std::shared_ptr<Buffer::Buffer> vertexBuffer, uint32_t vertexCount,
                               std::shared_ptr<Buffer::Buffer> indexBuffer, uint16_t indexCount ) {
  // Wait for fence
  auto fenceResult = vulkanDevice_.lock()->device.waitForFences( *inFlightFences[frameIndex_],
                                                                 vk::True, UINT64_MAX );
  if ( fenceResult != vk::Result::eSuccess ) {
    throw std::runtime_error( "Failed to wait for fence!" );
  }

  vk::Result result;
  uint32_t imageIndex;
  // Get swapchain image - Despite what the docs say, we need to catch this error to handle it
  try {
    auto res = swapChain_.lock()->swapChain.acquireNextImage(
        UINT64_MAX, *presentCompleteSemaphores[frameIndex_], nullptr );
    result = res.result;
    imageIndex = res.value;
  } catch ( const vk::OutOfDateKHRError &e ) {
    // Logger::log( "Caught out of date error while acquireing image.", Logger::INFO );
    result = vk::Result::eErrorOutOfDateKHR;
  }

  // Check for out of date swapchain
  if ( result == vk::Result::eErrorOutOfDateKHR ) {
    // Logger::log( "Out of date KHR, recreating swapchain", Logger::INFO );
    swapChain_.lock()->recreate_swap_chain();
    return;
  }

  // Check for early suboptimal KHR
  if ( result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR ) {
    Logger::log( "Failed to acquire swap chain image!", Logger::INFO );

    assert( result == vk::Result::eTimeout || result == vk::Result::eNotReady );
    throw std::runtime_error( "Failed to acquire swap chain image!" );
  }

  // Reset fence
  vulkanDevice_.lock()->device.resetFences( *inFlightFences[frameIndex_] );

  // Reset command buffer
  commandPool_->buffers[frameIndex_].reset();

  // Record buffer
  record_command_buffer( imageIndex, vertexBuffer, vertexCount, indexBuffer, indexCount );

  // Wait for queue
  vulkanDevice_.lock()
      ->graphicsQueue.waitIdle();  // NOTE: for simplicity, wait for the queue to be idle before
                                   // starting the frame In the next chapter you see how to use
                                   // multiple frames in flight and fences to sync

  // Create wait destination stage mask
  vk::PipelineStageFlags waitDestinationStageMask(
      vk::PipelineStageFlagBits::eColorAttachmentOutput );

  // Create submit info
  const vk::SubmitInfo submitInfo{ .waitSemaphoreCount = 1,
                                   .pWaitSemaphores = &*presentCompleteSemaphores[frameIndex_],
                                   .pWaitDstStageMask = &waitDestinationStageMask,
                                   .commandBufferCount = 1,
                                   .pCommandBuffers = &*commandPool_->buffers[frameIndex_],
                                   .signalSemaphoreCount = 1,
                                   .pSignalSemaphores = &*renderFinishedSemaphores[imageIndex] };

  // Submit to queue
  vulkanDevice_.lock()->graphicsQueue.submit( submitInfo, *inFlightFences[frameIndex_] );

  // Create present info
  const vk::PresentInfoKHR presentInfoKHR{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &*renderFinishedSemaphores[imageIndex],
      .swapchainCount = 1,
      .pSwapchains = &*swapChain_.lock()->swapChain,
      .pImageIndices = &imageIndex };

  // Queue present - Despite what the docs say, we seem to need to catch this error to handle it
  try {
    result = vulkanDevice_.lock()->graphicsQueue.presentKHR( presentInfoKHR );
  } catch ( const vk::OutOfDateKHRError &e ) {
    // Logger::log( "Caught out of date error while submiting present KHR.", Logger::INFO );
    result = vk::Result::eErrorOutOfDateKHR;
  }

  // Validate result / framebuffer resize
  if ( ( result == vk::Result::eSuboptimalKHR ) || ( result == vk::Result::eErrorOutOfDateKHR ) ||
       framebufferResized ) {
    framebufferResized = false;
    swapChain_.lock()->recreate_swap_chain();
  } else {
    // There are no other success codes than eSuccess; on any error code, presentKHR already threw
    // an exception.
    assert( result == vk::Result::eSuccess );
  }

  // Increment frame
  frameIndex_ = ( frameIndex_ + 1 ) % maxFramesInFlight_;
}

void VulkanRender::create_sync_objects() {
  Logger::log( "Creating sync objects...", Logger::DEBUG );

  // Validate sync objects
  assert( presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() &&
          inFlightFences.empty() );

  // Create sync objects

  for ( size_t i = 0; i < swapChain_.lock()->swapChainImages.size(); i++ ) {
    renderFinishedSemaphores.emplace_back( vulkanDevice_.lock()->device,
                                           vk::SemaphoreCreateInfo() );
  }

  for ( size_t i = 0; i < maxFramesInFlight_; i++ ) {
    presentCompleteSemaphores.emplace_back( vulkanDevice_.lock()->device,
                                            vk::SemaphoreCreateInfo() );
    inFlightFences.emplace_back(
        vulkanDevice_.lock()->device,
        vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled } );
  }

  //   presentCompleteSemaphore =
  //       vk::raii::Semaphore( vulkanDevice_.lock()->device, vk::SemaphoreCreateInfo() );
  //   renderFinishedSemaphore =
  //       vk::raii::Semaphore( vulkanDevice_.lock()->device, vk::SemaphoreCreateInfo() );
  //   drawFence = vk::raii::Fence( vulkanDevice_.lock()->device,
  //                                { .flags = vk::FenceCreateFlagBits::eSignaled } );
}

// ########################
// ###### DEPRECATED ######
// ########################

// void VulkanRender::destroy() {
//   for ( size_t i = 0; i < maxFramesInFlight_; i++ ) {
//     vkDestroySemaphore( vulkanDevice_->device, renderFinishedSemaphores_[i], nullptr );
//     vkDestroySemaphore( vulkanDevice_->device, imageAvailableSemaphores_[i], nullptr );
//     vkDestroyFence( vulkanDevice_->device, inFlightFences_[i], nullptr );
//   }
// }

// void VulkanRender::create_sync_objects() {
//   imageAvailableSemaphores_.resize( maxFramesInFlight_ );
//   renderFinishedSemaphores_.resize( maxFramesInFlight_ );
//   inFlightFences_.resize( maxFramesInFlight_ );

//   VkSemaphoreCreateInfo semaphoreInfo = Initializer::semaphore_info();

//   VkFenceCreateInfo fenceInfo = Initializer::fence_info();

//   for ( size_t i = 0; i < maxFramesInFlight_; i++ ) {
//     if ( vkCreateSemaphore( vulkanDevice_->device, &semaphoreInfo, nullptr,
//                             &imageAvailableSemaphores_[i] ) != VK_SUCCESS ||
//          vkCreateSemaphore( vulkanDevice_->device, &semaphoreInfo, nullptr,
//                             &renderFinishedSemaphores_[i] ) != VK_SUCCESS ||
//          vkCreateFence( vulkanDevice_->device, &fenceInfo, nullptr, &inFlightFences_[i] ) !=
//              VK_SUCCESS ) {
//       Logger::log( "Failed to create synchronization objects for a frame!", Logger::CRITICAL );
//     }
//   }
// }

// void VulkanRender::draw_frame( VkBuffer vertexBuffer, uint32_t vertexCount, VkBuffer
// indexBuffer,
//                                uint32_t indexCount, std::vector<void *> uniformBuffersMapped,
//                                std::vector<VkDescriptorSet> descriptorSets, VulkanImage
//                                *depthImage, VulkanImage *colorImage ) {
//   vkWaitForFences( vulkanDevice_->device, 1, &inFlightFences_[currentFrame_], VK_TRUE,
//   UINT64_MAX
//   );

//   uint32_t imageIndex;
//   VkResult result = vkAcquireNextImageKHR( vulkanDevice_->device, swapChain_->swapChain,
//   UINT64_MAX,
//                                            imageAvailableSemaphores_[currentFrame_],
//                                            VK_NULL_HANDLE, &imageIndex );

//   if ( result == VK_ERROR_OUT_OF_DATE_KHR ) {
//     swapChain_->recreate_swap_chain( depthImage, colorImage );
//     return;
//   } else if ( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR ) {
//     Logger::log( "Failed to acquire swap chain image!", Logger::CRITICAL );
//   }

//   update_uniform_buffer( currentFrame_, uniformBuffersMapped );

//   vkResetFences( vulkanDevice_->device, 1, &inFlightFences_[currentFrame_] );

//   vkResetCommandBuffer( commandBuffers_[currentFrame_],
//                         /*VkCommandBufferResetFlagBits*/ 0 );
//   record_command_buffer( commandBuffers_[currentFrame_], imageIndex, swapChain_, vertexBuffer,
//                          vertexCount, indexBuffer, indexCount, descriptorSets );

//   // vkAcquireNextImageKHR(vulkanDevice_->device, swapChain_->swapChain,
//   //                       UINT64_MAX, imageAvailableSemaphores_[currentFrame_],
//   //                       VK_NULL_HANDLE, &imageIndex);

//   // submit command buffer
//   VkSubmitInfo submitInfo{};
//   submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

//   VkSemaphore waitSemaphores[] = { imageAvailableSemaphores_[currentFrame_] };
//   VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
//   submitInfo.waitSemaphoreCount = 1;
//   submitInfo.pWaitSemaphores = waitSemaphores;
//   submitInfo.pWaitDstStageMask = waitStages;

//   submitInfo.commandBufferCount = 1;
//   submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];

//   VkSemaphore signalSemaphores[] = { renderFinishedSemaphores_[currentFrame_] };
//   submitInfo.signalSemaphoreCount = 1;
//   submitInfo.pSignalSemaphores = signalSemaphores;

//   if ( vkQueueSubmit( vulkanDevice_->graphicsQueue, 1, &submitInfo,
//                       inFlightFences_[currentFrame_] ) != VK_SUCCESS ) {
//     Logger::log( "Failed to submit draw command buffer!", Logger::CRITICAL );
//   }

//   VkPresentInfoKHR presentInfo{};
//   presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

//   presentInfo.waitSemaphoreCount = 1;
//   presentInfo.pWaitSemaphores = signalSemaphores;

//   VkSwapchainKHR swapChains[] = { swapChain_->swapChain };
//   presentInfo.swapchainCount = 1;
//   presentInfo.pSwapchains = swapChains;
//   presentInfo.pImageIndices = &imageIndex;
//   presentInfo.pResults = nullptr;  // Optional

//   result = vkQueuePresentKHR( vulkanDevice_->presentQueue, &presentInfo );

//   if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized_
//   )
//   {
//     framebufferResized_ = false;
//     swapChain_->recreate_swap_chain( depthImage, colorImage );
//   } else if ( result != VK_SUCCESS ) {
//     Logger::log( "Failed to present swap chain image!", Logger::CRITICAL );
//   }
// }

// void VulkanRender::record_command_buffer( VkCommandBuffer commandBuffer, uint32_t imageIndex,
//                                           VulkanSwapChain *swapChain, VkBuffer vertexBuffer,
//                                           uint32_t vertexCoundeptht, VkBuffer indexBuffer,
//                                           uint32_t indexCount,
//                                           std::vector<VkDescriptorSet> descriptorSets ) {
//   VkCommandBufferBeginInfo beginInfo = Initializer::command_buffer_begin_info();

//   if ( vkBeginCommandBuffer( commandBuffer, &beginInfo ) != VK_SUCCESS ) {
//     Logger::log( "Failed to begin recording command buffer!", Logger::CRITICAL );
//   }

//   VkRenderPassBeginInfo renderPassInfo = Initializer::render_pass_info(
//       swapChain->renderPass, swapChain->swapChainFramebuffers[imageIndex], swapChain->extent );

//   std::array<VkClearValue, 2> clearValues{};
//   clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
//   clearValues[1].depthStencil = { 1.0f, 0 };

//   renderPassInfo.clearValueCount = static_cast<uint32_t>( clearValues.size() );
//   renderPassInfo.pClearValues = clearValues.data();

//   // VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };  // background color
//   // renderPassInfo.clearValueCount = 1;
//   // renderPassInfo.pClearValues = &clearColor;

//   vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

//   vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
//   pipeline_->graphicsPipeline
//   );

//   VkViewport viewport = Initializer::viewport( static_cast<float>( swapChain->extent.height ),
//                                                static_cast<float>( swapChain->extent.width ) );
//   vkCmdSetViewport( commandBuffer, 0, 1, &viewport );

//   VkRect2D scissor = Initializer::scissor( swapChain->extent );
//   // VkRect2D scissor{};
//   // scissor.offset = {0, 0};
//   // scissor.extent = swapChain->extent;
//   vkCmdSetScissor( commandBuffer, 0, 1, &scissor );

//   VkBuffer vertexBuffers[] = { vertexBuffer };
//   VkDeviceSize offsets[] = { 0 };
//   vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );

//   vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );

//   // vkCmdDraw( commandBuffer, vertexCount, 1, 0, 0 );

//   vkCmdBindIndexBuffer( commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16 );

//   vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
//                            pipeline_->pipelineLayout, 0, 1, &descriptorSets[currentFrame_], 0,
//                            nullptr );

//   vkCmdDrawIndexed( commandBuffer, indexCount, 1, 0, 0, 0 );
//   vkCmdEndRenderPass( commandBuffer );

//   if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS ) {
//     Logger::log( "Failed to record command buffer!", Logger::CRITICAL );
//   }
// }

// void VulkanRender::update_uniform_buffer( uint32_t currentImage,
//                                           std::vector<void *> uniformBuffersMapped ) {
//   static auto startTime = std::chrono::high_resolution_clock::now();

//   auto currentTime = std::chrono::high_resolution_clock::now();
//   float run_time =
//       std::chrono::duration<float, std::chrono::seconds::period>( currentTime - startTime
//       ).count();
//   // Logger::log( "Run time: " + std::to_string( run_time ), Logger::DEBUG );

//   UniformBufferObject ubo{};
//   ubo.model = glm::rotate( glm::mat4( 1.0f ), run_time * glm::radians( 90.0f ),
//                            glm::vec3( 0.0f, 0.0f, 1.0f ) );
//   ubo.view = glm::lookAt( glm::vec3( 2.0f, 2.0f, 2.0f ), glm::vec3( 0.0f, 0.0f, 0.0f ),
//                           glm::vec3( 0.0f, 0.0f, 1.0f ) );
//   ubo.proj =
//       glm::perspective( glm::radians( 45.0f ),
//                         swapChain_->extent.width / (float)swapChain_->extent.height,
//                         0.1f, 10.0f
//                         );
//   ubo.proj[1][1] *= -1;
//   memcpy( uniformBuffersMapped[currentImage], &ubo, sizeof( ubo ) );
// }

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
