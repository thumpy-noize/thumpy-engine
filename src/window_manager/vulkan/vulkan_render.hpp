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

#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include "vulkan_buffers.hpp"
#include "vulkan_construct.hpp"
#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"
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
  VulkanRender( std::shared_ptr<VulkanDevice> vulkanDevice,
                std::shared_ptr<VulkanSwapChain> swapChain,
                std::shared_ptr<VulkanPipeline> pipeline,
                std::shared_ptr<Construct::CommandPool> commandPool, int maxFramesInFlight );

  void record_command_buffer( uint32_t imageIndex, std::shared_ptr<Buffer::Buffer> vertexBuffer,
                              uint32_t vertexCount, std::shared_ptr<Buffer::Buffer> indexBuffer,
                              uint16_t indexCount, std::shared_ptr<Image::VulkanImage> depthImage,
                              std::shared_ptr<Image::VulkanImage> colorImage,
                              std::shared_ptr<Descriptors> descriptors );

  // NOTE: vulkan_image has a function with the same name as this. I don't like that...
  void transition_image_layout( vk::Image image, vk::ImageLayout old_layout,
                                vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask,
                                vk::AccessFlags2 dst_access_mask,
                                vk::PipelineStageFlags2 src_stage_mask,
                                vk::PipelineStageFlags2 dst_stage_mask,
                                vk::ImageAspectFlags image_aspect_flags );

  // /**
  //  * @brief Destroy render
  //  *
  //  */
  // void destroy();

  void create_sync_objects();

  // /**
  //  * @brief Draw to frame
  //  *
  //  */
  void draw_frame( bool& framebufferResized, std::shared_ptr<Buffer::Buffer> vertexBuffer,
                   uint32_t vertexCount, std::shared_ptr<Buffer::Buffer> indexBuffer,
                   uint16_t indexCount, std::vector<void*> uniformBuffersMapped,
                   std::shared_ptr<Image::VulkanImage> depthImage,
                   std::shared_ptr<Image::VulkanImage> colorImage,
                   std::shared_ptr<Descriptors> descriptors );

  // void draw_frame( VkBuffer vertexBuffer, uint32_t vertexCount, VkBuffer indexBuffer,
  //                  uint32_t indexCount, std::vector<void *> uniformBuffersMapped,
  //                  std::vector<VkDescriptorSet> descriptorSets, VulkanImage *depthImage,
  //                  VulkanImage *colorImage );

  // void record_command_buffer( VkCommandBuffer commandBuffer, uint32_t imageIndex,
  //                             VulkanSwapChain *swapChain, VkBuffer vertexBuffer,
  //                             uint32_t vertexCount, VkBuffer indexBuffer, uint32_t indexCount,
  //                             std::vector<VkDescriptorSet> descriptorSets );

  void update_uniform_buffer( uint32_t currentImage, std::vector<void*> uniformBuffersMapped );

 protected:
  uint32_t maxFramesInFlight_;  // Max frames in flight
  uint32_t frameIndex_ = 0;     // Current frame index

  // std::shared_ptr<bool> framebufferResized_;  // Resize callback ptr

  std::weak_ptr<VulkanDevice> vulkanDevice_;             // prt to vulkan device
  std::weak_ptr<VulkanSwapChain> swapChain_;             // ptr to swap chain
  std::weak_ptr<VulkanPipeline> pipeline_;               // ptr to pipeline
  std::shared_ptr<Construct::CommandPool> commandPool_;  // ptr to command pool / buffers

  std::vector<vk::raii::Semaphore> presentCompleteSemaphores;  // Present sync object
  std::vector<vk::raii::Semaphore> renderFinishedSemaphores;   // Render sync object
  std::vector<vk::raii::Fence> inFlightFences;                 // In flight fences

  // int maxFramesInFlight_;
  // uint32_t currentFrame_ = 0;

  // VulkanDevice *vulkanDevice_;
  // VulkanSwapChain *swapChain_;
  // VulkanPipeline *pipeline_;
  // bool framebufferResized_;

  // std::vector<VkCommandBuffer> commandBuffers_;

  // std::vector<VkSemaphore> imageAvailableSemaphores_;
  // std::vector<VkSemaphore> renderFinishedSemaphores_;
  // std::vector<VkFence> inFlightFences_;

  // VkSemaphore imageAvailableSemaphore_;
  // VkSemaphore renderFinishedSemaphore_;
};
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
