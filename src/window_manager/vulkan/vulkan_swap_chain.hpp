#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <vector>

#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

class VulkanSwapChain {
 public:
  VulkanSwapChain( VulkanDevice *vulkanDevice, GLFWwindow *window,
                   VkSurfaceKHR surface );

  /**
   * @brief Create swap chain
   */
  void create_swap_chain();
  /**
   * @brief Recreate swap chain
   */
  void recreate_swap_chain();
  /**
   * @brief Clear the swap chain
   */
  void clear_swap_chain();

  /**
   * @brief Get swap chain details
   * @return SwapChainSupportDetails
   */
  SwapChainSupportDetails query_swap_chain_support();

  /**
   * @brief Chose Best available format
   *
   * @param availableFormats
   * @return VkSurfaceFormatKHR
   */
  VkSurfaceFormatKHR choose_swap_surface_format(
      const std::vector<VkSurfaceFormatKHR> &availableFormats );

  /** @brief Chose Best available present mode
   *  Currently using mailbox
   * @param availableFormats
   * @return VkSurfaceFormatKHR
   */
  VkPresentModeKHR choose_swap_present_mode(
      const std::vector<VkPresentModeKHR> &availablePresentModes );

  /**
   * @brief get window extent
   *
   * @param capabilities
   * @return VkExtent2D
   */
  VkExtent2D choose_swap_extent( const VkSurfaceCapabilitiesKHR &capabilities );

  void create_image_views( VkDevice device );
  void create_framebuffers();
  void create_render_pass();

 public:
  VkSwapchainKHR swapChain;

  VkFormat swapChainImageFormat;
  VkExtent2D extent;
  VkRenderPass renderPass;

  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;
  std::vector<VkImage> swapChainImages;

 private:
  VkInstance instance_;
  VkSurfaceKHR surface_;
  GLFWwindow *window_;
  VulkanDevice *vulkanDevice_;
};
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
