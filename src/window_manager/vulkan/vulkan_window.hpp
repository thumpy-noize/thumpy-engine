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
#include "vulkan/vulkan_render.hpp"
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

  /**
   * @brief Setup vulkan
   *
   */
  void init_vulkan();

  /**
   * @brief deconstruct vulkan window
   *
   */
  void deconstruct_window();

  /**
   * @brief Render loop
   *
   */
  void loop();

  void create_instance();
  void create_surface();

#pragma endregion Core

private:
  const int MAX_FRAMES_IN_FLIGHT = 2;
  uint32_t currentFrame_ = 0;

  VkInstance instance_;
  VkSurfaceKHR surface_;

  VulkanDevice *vulkanDevice_;
  VulkanSwapChain *swapChain_;
  VulkanPipeline pipeline_;
  VulkanRender *render_;

  VkCommandPool commandPool_;
  std::vector<VkCommandBuffer> commandBuffers_;
  VkDebugUtilsMessengerEXT debugMessenger_;

  // const std::vector<Vertex> vertices_ = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
  //                                        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
  //                                        {{-0.5f, 0.5f}, {0.0f,
  //                                        0.0f, 1.0f}}};
};
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
