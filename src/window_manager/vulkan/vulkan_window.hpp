#pragma once

#include "vulkan/vulkan_debug.hpp"
#include "vulkan/vulkan_device.hpp"
#include "vulkan/vulkan_swap_chain.hpp"
#include "window.hpp"
#include <optional>
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
  void create_instance();
  void deconstruct_window();

  void loop();

#pragma endregion Core

  void setup_debug_messenger();
  void create_surface();

#pragma region Image

  void create_graphics_pipeline();
  VkShaderModule create_shader_module(const std::vector<char> &code);
  void create_framebuffers();
  void create_command_pool();
  void create_command_buffer();
  void record_command_buffer(VkCommandBuffer commandBuffer,
                             uint32_t imageIndex);
  void draw_frame();

  void create_sync_objects();

#pragma endregion Image

  std::vector<const char *> get_required_extensions();

  bool check_validation_layer_support();
  void populate_debug_messenger_create_info(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

private:
  const int MAX_FRAMES_IN_FLIGHT = 2;

  Device::VulkanDevice *vulkanDevice_;
  VulkanSwapChain *swapChain_;

  VkInstance instance_;
  VkSurfaceKHR surface_;

  VkPipelineLayout pipelineLayout_;
  VkPipeline graphicsPipeline_;

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
