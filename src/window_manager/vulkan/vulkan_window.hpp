/**
 * @file vulkan_window.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Vulkan support for our glfw window.
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "vulkan/vulkan_device.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_swap_chain.hpp"
#include "window.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

class VulkanWindow : public Window {
public:
  VulkanWindow(std::string title);

#pragma region Core

  void init_vulkan();
  void deconstruct_window();

  void create_instance();
  void create_surface();

  void loop();

#pragma endregion Core

#pragma region Image

  void draw_frame();

  void create_sync_objects();

#pragma endregion Image

private:
  const int MAX_FRAMES_IN_FLIGHT = 2;

  VkInstance instance_;
  VkSurfaceKHR surface_;

  VulkanDevice *vulkanDevice_;
  VulkanSwapChain *swapChain_;
  VulkanPipeline pipeline_;

  VkCommandPool commandPool_;
  std::vector<VkCommandBuffer> commandBuffers_;

  std::vector<VkSemaphore> imageAvailableSemaphores_;
  std::vector<VkSemaphore> renderFinishedSemaphores_;
  std::vector<VkFence> inFlightFences_;

  uint32_t currentFrame_ = 0;

  VkSemaphore imageAvailableSemaphore_;
  VkSemaphore renderFinishedSemaphore_;

  VkDebugUtilsMessengerEXT debugMessenger_;
};
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
