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

#include "vulkan/vulkan_device.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_swap_chain.hpp"
namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
class VulkanRender {
 public:
  /**
   * @brief Construct and setup a new Vulkan Render object.
   *
   * @param max_frames_in_fligh
   * @param vulkanDevice
   * @param swapchain
   * @param commandBuffers
   * @param pipeline
   */
  VulkanRender( int max_frames_in_fligh, VulkanDevice *vulkanDevice,
                VulkanSwapChain *swapchain,
                std::vector<VkCommandBuffer> *commandBuffers,
                VulkanPipeline pipeline );

  /**
   * @brief Destroy render
   *
   */
  void destroy();

  /**
   * @brief Set the context for this render
   *
   * @param max_frames_in_flight
   * @param vulkanDevice
   * @param swapchain
   * @param commandBuffers
   * @param pipeline
   */
  void set_context( int max_frames_in_flight, VulkanDevice *vulkanDevice,
                    VulkanSwapChain *swapchain,
                    std::vector<VkCommandBuffer> *commandBuffers,
                    VulkanPipeline pipeline );

  void create_sync_objects();

  /**
   * @brief Draw to frame
   *
   */
  void draw_frame( VkBuffer vertexBuffer, uint32_t vertexCount );

  void record_command_buffer( VkCommandBuffer commandBuffer,
                              uint32_t imageIndex, VulkanSwapChain *swapChain,
                              VkBuffer vertexBuffer, uint32_t vertexCount );

 protected:
  int maxFramesInFlight_;
  uint32_t currentFrame_ = 0;

  VulkanDevice *vulkanDevice_;
  VulkanSwapChain *swapChain_;
  VulkanPipeline pipeline_;
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
