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

#pragma once
#include <vulkan/vulkan_core.h>

#include <vector>

#include "vulkan/vulkan_helper.hpp"
#include "vulkan_device.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_swap_chain.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
class VulkanRender {
 public:
  /**
   * @brief Construct and setup a new Vulkan Render object.
   *
   * @param max_frames_in_flight
   * @param vulkanDevice
   * @param swapchain
   * @param commandBuffers
   * @param pipeline
   */
  VulkanRender( int maxFramesInFlight, VulkanDevice *vulkanDevice, VulkanSwapChain *swapchain,
                std::vector<VkCommandBuffer> *commandBuffers, VulkanPipeline *pipeline );

  /**
   * @brief Destroy render
   *
   */
  void destroy();

  void create_sync_objects();

  /**
   * @brief Draw to frame
   *
   */
  void draw_frame( VkBuffer vertexBuffer, uint32_t vertexCount, VkBuffer indexBuffer,
                   uint32_t indexCount, std::vector<void *> uniformBuffersMapped,
                   std::vector<VkDescriptorSet> descriptorSets, VulkanImage *depthImage,
                   VulkanImage *colorImage );

  void record_command_buffer( VkCommandBuffer commandBuffer, uint32_t imageIndex,
                              VulkanSwapChain *swapChain, VkBuffer vertexBuffer,
                              uint32_t vertexCount, VkBuffer indexBuffer, uint32_t indexCount,
                              std::vector<VkDescriptorSet> descriptorSets );

  void update_uniform_buffer( uint32_t currentImage, std::vector<void *> uniformBuffersMapped );

 protected:
  int maxFramesInFlight_;
  uint32_t currentFrame_ = 0;

  VulkanDevice *vulkanDevice_;
  VulkanSwapChain *swapChain_;
  VulkanPipeline *pipeline_;
  bool framebufferResized_;

  std::vector<VkCommandBuffer> commandBuffers_;

  std::vector<VkSemaphore> imageAvailableSemaphores_;
  std::vector<VkSemaphore> renderFinishedSemaphores_;
  std::vector<VkFence> inFlightFences_;

  VkSemaphore imageAvailableSemaphore_;
  VkSemaphore renderFinishedSemaphore_;
};
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
