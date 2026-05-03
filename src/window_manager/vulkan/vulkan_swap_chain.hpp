/**
 * @file vulkan_swap_chain.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Vulkan swapchain variables and methods
 * @version 0.1
 * @date 2024-12-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <GLFW/glfw3.h>

#include <memory>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_image.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

class VulkanSwapChain : public std::enable_shared_from_this<VulkanSwapChain> {
 public:
  VulkanSwapChain( std::shared_ptr<VulkanDevice> vulkanDevice, GLFWwindow *window,
                   std::shared_ptr<vk::raii::SurfaceKHR> surface );

  //   ~VulkanSwapChain();

  /**
   * @brief Create swap chain
   */
  void create_swap_chain();

  /**
   * @brief Recreate swap chain
   */
  void recreate_swap_chain( std::shared_ptr<Image::VulkanImage> depthImage,
                            std::shared_ptr<Image::VulkanImage> colorImage );

  /**
   * @brief Clear the swap chain
   */
  void clear_swap_chain();

  /**
   * @brief Get swap chain details
   * @return SwapChainSupportDetails
   */
  //   SwapChainSupportDetails query_swap_chain_support();

  /**
   * @brief Chose Best available format
   *
   * @param availableFormats
   * @return VkSurfaceFormatKHR
   */
  vk::SurfaceFormatKHR choose_swap_surface_format(
      const std::vector<vk::SurfaceFormatKHR> &availableFormats );

  /** @brief Chose Best available present mode
   *  Currently using mailbox
   * @param availableFormats
   * @return VkSurfaceFormatKHR
   */
  vk::PresentModeKHR choose_swap_present_mode(
      const std::vector<vk::PresentModeKHR> &availablePresentModes );

  /**
   * @brief get window extent
   *
   * @param capabilities
   * @return VkExtent2D
   */
  vk::Extent2D choose_swap_extent( const vk::SurfaceCapabilitiesKHR &capabilities );

  uint32_t choose_swap_min_image_count( vk::SurfaceCapabilitiesKHR const &surfaceCapabilities );

  void create_image_views();

 public:
  vk::raii::SwapchainKHR swapChain = nullptr;
  vk::SurfaceFormatKHR swapChainSurfaceFormat;
  vk::Extent2D swapChainExtent;

  std::vector<vk::raii::ImageView> swapChainImageViews;
  std::vector<vk::Image> swapChainImages;

 private:
  std::weak_ptr<vk::raii::SurfaceKHR> surface_;  // ptr to surface
  GLFWwindow *window_;                           // ptr to existing window // TODO: Make weak ptr
  std::weak_ptr<VulkanDevice> vulkanDevice_;

  //   std::vector<VkImage> swapChainImages_;
};
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
