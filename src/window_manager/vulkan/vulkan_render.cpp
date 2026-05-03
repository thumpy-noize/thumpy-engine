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
}

void VulkanRender::record_command_buffer(
    uint32_t imageIndex, std::shared_ptr<Buffer::Buffer> vertexBuffer, uint32_t vertexCount,
    std::shared_ptr<Buffer::Buffer> indexBuffer, uint16_t indexCount,
    std::shared_ptr<Image::VulkanImage> depthImage, std::shared_ptr<Image::VulkanImage> colorImage,
    std::shared_ptr<Descriptors> descriptors ) {
  // Get current buffer
  auto &commandBuffer = commandPool_->buffers[frameIndex_];

  // Begin command buffer
  commandBuffer.begin( {} );

  // Transistion swap chain for color attachment
  transition_image_layout( swapChain_.lock()->swapChainImages[imageIndex],
                           vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
                           {},  // srcAccessMask (no need to wait for previous operations)
                           vk::AccessFlagBits2::eColorAttachmentWrite,          // dstAccessMask
                           vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // srcStage
                           vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // dstStage
                           vk::ImageAspectFlagBits::eColor                      // Aspect flags
  );

  // Transition the multisampled color image to COLOR_ATTACHMENT_OPTIMAL
  transition_image_layout(
      *colorImage->image, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
      vk::AccessFlagBits2::eColorAttachmentWrite, vk::AccessFlagBits2::eColorAttachmentWrite,
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,
      vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::ImageAspectFlagBits::eColor );

  // Transition depth image to depth attachment optimal layout
  transition_image_layout( *depthImage->image, vk::ImageLayout::eUndefined,
                           vk::ImageLayout::eDepthAttachmentOptimal,
                           vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
                           vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
                           vk::PipelineStageFlagBits2::eEarlyFragmentTests |
                               vk::PipelineStageFlagBits2::eLateFragmentTests,
                           vk::PipelineStageFlagBits2::eEarlyFragmentTests |
                               vk::PipelineStageFlagBits2::eLateFragmentTests,
                           vk::ImageAspectFlagBits::eDepth );

  // Set clear color / depth
  vk::ClearValue clearColor = vk::ClearColorValue( 0.0f, 0.0f, 0.0f, 1.0f );
  vk::ClearValue clearDepth = vk::ClearDepthStencilValue( 1.0f, 0 );

  // Create color attachment info
  vk::RenderingAttachmentInfo colorAttachmentInfo = {
      .imageView = colorImage->imageView,
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .resolveMode = vk::ResolveModeFlagBits::eAverage,
      .resolveImageView = swapChain_.lock()->swapChainImageViews[imageIndex],
      .resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = clearColor };

  // Create depth attachment info
  vk::RenderingAttachmentInfo depthAttachmentInfo = {
      .imageView = depthImage->imageView,
      .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eDontCare,
      .clearValue = clearDepth };

  // Create rendering info
  vk::RenderingInfo renderingInfo = {
      .renderArea = { .offset = { 0, 0 }, .extent = swapChain_.lock()->swapChainExtent },
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorAttachmentInfo,
      .pDepthAttachment = &depthAttachmentInfo };

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
  commandBuffer.bindIndexBuffer( *indexBuffer->buffer, 0, vk::IndexType::eUint32 );

  // Bind descriptor sets
  commandBuffer.bindDescriptorSets( vk::PipelineBindPoint::eGraphics,
                                    pipeline_.lock()->pipelineLayout, 0,
                                    *descriptors->sets[frameIndex_], nullptr );

  // Draw buffer
  commandBuffer.drawIndexed( indexCount, 1, 0, 0, 0 );

  // End rendering
  commandBuffer.endRendering();

  // Transition to image
  transition_image_layout( swapChain_.lock()->swapChainImages[imageIndex],
                           vk::ImageLayout::eColorAttachmentOptimal,
                           vk::ImageLayout::ePresentSrcKHR,
                           vk::AccessFlagBits2::eColorAttachmentWrite,          // srcAccessMask
                           {},                                                  // dstAccessMask
                           vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // srcStage
                           vk::PipelineStageFlagBits2::eBottomOfPipe,           // dstStage
                           vk::ImageAspectFlagBits::eColor                      // aspectFlags
  );

  // End command buffer
  commandBuffer.end();
}

void VulkanRender::transition_image_layout( vk::Image image, vk::ImageLayout old_layout,
                                            vk::ImageLayout new_layout,
                                            vk::AccessFlags2 src_access_mask,
                                            vk::AccessFlags2 dst_access_mask,
                                            vk::PipelineStageFlags2 src_stage_mask,
                                            vk::PipelineStageFlags2 dst_stage_mask,
                                            vk::ImageAspectFlags image_aspect_flags ) {
  vk::ImageMemoryBarrier2 barrier = { .srcStageMask = src_stage_mask,
                                      .srcAccessMask = src_access_mask,
                                      .dstStageMask = dst_stage_mask,
                                      .dstAccessMask = dst_access_mask,
                                      .oldLayout = old_layout,
                                      .newLayout = new_layout,
                                      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                      .image = image,
                                      .subresourceRange = { .aspectMask = image_aspect_flags,
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
                               std::shared_ptr<Buffer::Buffer> indexBuffer, uint16_t indexCount,
                               std::vector<void *> uniformBuffersMapped,
                               std::shared_ptr<Image::VulkanImage> depthImage,
                               std::shared_ptr<Image::VulkanImage> colorImage,
                               std::shared_ptr<Descriptors> descriptors ) {
  // Wait for fence
  auto fenceResult = vulkanDevice_.lock()->device.waitForFences( *inFlightFences[frameIndex_],
                                                                 vk::True, UINT64_MAX );
  if ( fenceResult != vk::Result::eSuccess ) {
    throw VulkanRuntimeError( "Failed to wait for fence!" );
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
    swapChain_.lock()->recreate_swap_chain( depthImage, colorImage );
    return;
  }

  // Check for early suboptimal KHR
  if ( result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR ) {
    Logger::log( "Failed to acquire swap chain image!", Logger::INFO );

    assert( result == vk::Result::eTimeout || result == vk::Result::eNotReady );
    throw VulkanRuntimeError( "Failed to acquire swap chain image!" );
  }

  // Update uniform buffers
  update_uniform_buffer( frameIndex_, uniformBuffersMapped );

  // Reset fence
  vulkanDevice_.lock()->device.resetFences( *inFlightFences[frameIndex_] );

  // Reset command buffer
  commandPool_->buffers[frameIndex_].reset();

  // Record buffer
  record_command_buffer( imageIndex, vertexBuffer, vertexCount, indexBuffer, indexCount, depthImage,
                         colorImage, descriptors );

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
    swapChain_.lock()->recreate_swap_chain( depthImage, colorImage );
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
}

void VulkanRender::update_uniform_buffer( uint32_t currentImage,
                                          std::vector<void *> uniformBuffersMapped ) {
  // Get start time
  static auto startTime = std::chrono::high_resolution_clock::now();

  // Get current time
  auto currentTime = std::chrono::high_resolution_clock::now();

  // Get time difference
  float time =
      std::chrono::duration<float, std::chrono::seconds::period>( currentTime - startTime ).count();

  // Create uniform buffer object
  UniformBufferObject ubo{};
  ubo.model =
      rotate( glm::mat4( 1.0f ), time * glm::radians( 90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
  ubo.view = lookAt( glm::vec3( 2.0f, 2.0f, 2.0f ), glm::vec3( 0.0f, 0.0f, 0.0f ),
                     glm::vec3( 0.0f, 0.0f, 1.0f ) );
  ubo.proj = glm::perspective( glm::radians( 45.0f ),
                               static_cast<float>( swapChain_.lock()->swapChainExtent.width ) /
                                   static_cast<float>( swapChain_.lock()->swapChainExtent.height ),
                               0.1f, 10.0f );
  ubo.proj[1][1] *= -1;

  memcpy( uniformBuffersMapped[currentImage], &ubo, sizeof( ubo ) );
}

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
