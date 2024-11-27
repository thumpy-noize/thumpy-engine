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

// struct QueueFamilyIndices {
//   std::optional<uint32_t> graphicsFamily;
//   std::optional<uint32_t> presentFamily;

//   bool is_complete() {
//     return graphicsFamily.has_value() && presentFamily.has_value();
//   }
// };

// struct SwapChainSupportDetails {
//   VkSurfaceCapabilitiesKHR capabilities;
//   std::vector<VkSurfaceFormatKHR> formats;
//   std::vector<VkPresentModeKHR> presentModes;
// };

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

#pragma region Swapchain

  void create_swap_chain();
  void recreate_swap_chain();
  void clear_swap_chain();

  VkSurfaceFormatKHR choose_swap_surface_format(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR choose_swap_present_mode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities);
  /// Finds swapchain details
  SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);

#pragma endregion Swapchain

#pragma region Image

  void create_image_views();
  void create_graphics_pipeline();
  VkShaderModule create_shader_module(const std::vector<char> &code);
  void create_render_pass();
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

  SwapChain *newSwapChain_;
  Device::VulkanDevice *vulkanDevice_;

  VkInstance instance_;
  VkSurfaceKHR surface_;

  VkSwapchainKHR swapChain_;
  std::vector<VkImage> swapChainImages_;
  VkFormat swapChainImageFormat_;
  VkExtent2D swapChainExtent_;
  std::vector<VkImageView> swapChainImageViews_;
  std::vector<VkFramebuffer> swapChainFramebuffers_;

  VkRenderPass renderPass_;
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
