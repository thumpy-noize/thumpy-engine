#pragma once

#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

class VulkanSwapChain {
public:
  VulkanSwapChain(VkInstance instance, VulkanDevice *Device,
                  VkSurfaceKHR surface, GLFWwindow *window);
  /**
   * @brief Set swap chain context variables
   *
   * @param instance
   * @param device
   * @param surface
   * @param window
   */
  void set_context(VkInstance instance, VulkanDevice *device,
                   VkSurfaceKHR surface, GLFWwindow *window);
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
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  /** @brief Chose Best available present mode
   *  Currently using mailbox
   * @param availableFormats
   * @return VkSurfaceFormatKHR
   */
  VkPresentModeKHR choose_swap_present_mode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  /**
   * @brief get window extent
   *
   * @param capabilities
   * @return VkExtent2D
   */
  VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities);
  QueueFamilyIndices find_queue_families();

  void create_image_views();
  void create_framebuffers();
  void create_render_pass();

public:
  VkSwapchainKHR swapChain;

  VkFormat swapChainImageFormat;
  VkExtent2D extent;
  VkRenderPass renderPass;

  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

private:
  VkInstance instance_;
  VkSurfaceKHR surface_;
  GLFWwindow *window_;
  VulkanDevice *device_;

  std::vector<VkImage> swapChainImages_;
};
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
