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

  void create_vertex_buffer();

  uint32_t find_memory_type(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties);

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

<<<<<<< HEAD
  // const std::vector<Vertex> vertices_ = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
  //                                        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
  //                                        {{-0.5f, 0.5f}, {0.0f,
  //                                        0.0f, 1.0f}}};
=======
  const std::vector<Vertex> vertices_ = {
      {{0.0f, -1.0f}, {1.0f, 0.0f, 1.0f}}, {{1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      {{-1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, {{0.0f, -0.9f}, {1.0f, 0.0f, 1.0f}},
      {{0.9f, 0.9f}, {0.0f, 1.0f, 0.0f}},  {{-0.9f, 0.9f}, {0.0f, 0.0f, 1.0f}},
      {{0.0f, -0.8f}, {1.0f, 0.0f, 1.0f}}, {{0.8f, 0.8f}, {0.0f, 1.0f, 0.0f}},
      {{-0.8f, 0.8f}, {0.0f, 0.0f, 1.0f}}, {{0.0f, -0.7f}, {1.0f, 0.0f, 1.0f}},
      {{0.7f, 0.7f}, {0.0f, 1.0f, 0.0f}},  {{-0.7f, 0.7f}, {0.0f, 0.0f, 1.0f}},
      {{0.0f, -0.6f}, {1.0f, 0.0f, 1.0f}}, {{0.6f, 0.6f}, {0.0f, 1.0f, 0.0f}},
      {{-0.6f, 0.6f}, {0.0f, 0.0f, 1.0f}}, {{0.0f, -0.5f}, {1.0f, 0.0f, 1.0f}},
      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{0.0f, -0.4f}, {1.0f, 0.0f, 1.0f}}, {{0.4f, 0.4f}, {0.0f, 1.0f, 0.0f}},
      {{-0.4f, 0.4f}, {0.0f, 0.0f, 1.0f}}, {{0.0f, -0.3f}, {1.0f, 0.0f, 1.0f}},
      {{0.3f, 0.3f}, {0.0f, 1.0f, 0.0f}},  {{-0.3f, 0.3f}, {0.0f, 0.0f, 1.0f}},
      {{0.0f, -0.2f}, {1.0f, 0.0f, 1.0f}}, {{0.2f, 0.2f}, {0.0f, 1.0f, 0.0f}},
      {{-0.2f, 0.2f}, {0.0f, 0.0f, 1.0f}}, {{0.0f, -0.1f}, {1.0f, 0.0f, 1.0f}},
      {{0.1f, 0.1f}, {0.0f, 1.0f, 0.0f}},  {{-0.1f, 0.1f}, {0.0f, 0.0f, 1.0f}}};

  VkBuffer vertexBuffer_;
  VkDeviceMemory vertexBufferMemory_;
>>>>>>> 0cd495308e (Fake Recurisve triange)
};
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
