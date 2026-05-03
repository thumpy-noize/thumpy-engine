/**
 * @file vulkan_swap_chain.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief Vulkan swapchain cpp file
 * @version 0.1
 * @date 2024-12-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vulkan_swap_chain.hpp"

// #include <vulkan/vulkan_core.h>
#include <algorithm>  // Necessary for std::clamp
#include <limits>
#include <vulkan/vulkan_raii.hpp>

#include "logger.hpp"
// #include "vulkan_buffers.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

VulkanSwapChain::VulkanSwapChain( std::shared_ptr<VulkanDevice> vulkanDevice, GLFWwindow *window,
                                  std::shared_ptr<vk::raii::SurfaceKHR> surface ) {
  Logger::log( "Constructing Vulkan swap chain...", Logger::DEBUG );

  // Get weak ptr to vulkan device
  vulkanDevice_ = vulkanDevice;

  // Get ptr to window // TODO: make weak ptr
  window_ = window;

  // Get weak ptr to surface
  surface_ = surface;

  // Create swap chain
  create_swap_chain();

  // Create image views
  create_image_views();
}

void VulkanSwapChain::create_swap_chain() {
  // Logger::log( "Creating swap chain...", Logger::DEBUG );

  // Get surface capabilities
  vk::SurfaceCapabilitiesKHR surfaceCapabilities =
      vulkanDevice_.lock()->physicalDevice.getSurfaceCapabilitiesKHR( *surface_.lock() );

  // Get extent
  swapChainExtent = choose_swap_extent( surfaceCapabilities );

  // Get min image count
  uint32_t minImageCount = choose_swap_min_image_count( surfaceCapabilities );

  // Get surface format
  std::vector<vk::SurfaceFormatKHR> availableFormats =
      vulkanDevice_.lock()->physicalDevice.getSurfaceFormatsKHR( *surface_.lock() );
  swapChainSurfaceFormat = choose_swap_surface_format( availableFormats );

  // Get present mode
  std::vector<vk::PresentModeKHR> availablePresentModes =
      vulkanDevice_.lock()->physicalDevice.getSurfacePresentModesKHR( *surface_.lock() );
  vk::PresentModeKHR presentMode = choose_swap_present_mode( availablePresentModes );

  vk::SwapchainCreateInfoKHR swapChainCreateInfo{
      .surface = *surface_.lock(),
      .minImageCount = minImageCount,
      .imageFormat = swapChainSurfaceFormat.format,
      .imageColorSpace = swapChainSurfaceFormat.colorSpace,
      .imageExtent = swapChainExtent,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
      .imageSharingMode = vk::SharingMode::eExclusive,
      .preTransform = surfaceCapabilities.currentTransform,
      .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
      .presentMode = presentMode,
      .clipped = true };

  swapChain = vk::raii::SwapchainKHR( vulkanDevice_.lock()->device, swapChainCreateInfo );
  swapChainImages = swapChain.getImages();
}

void VulkanSwapChain::recreate_swap_chain( std::shared_ptr<Image::VulkanImage> depthImage,
                                           std::shared_ptr<Image::VulkanImage> colorImage ) {
  // Logger::log( "Recreating swap chain...", Logger::DEBUG );

  // Get new window dimensions
  int width = 0, height = 0;
  // glfwGetFramebufferSize( window_, &width, &height );
  while ( width == 0 || height == 0 ) {
    glfwGetFramebufferSize( window_, &width, &height );
    glfwWaitEvents();
  }

  // Wait for device
  vulkanDevice_.lock()->device.waitIdle();

  // Clear existing swap chain
  clear_swap_chain();

  // Create swap chain
  create_swap_chain();

  // Create image views
  create_image_views();

  // Create msaa resources
  Image::create_color_resources( colorImage, vulkanDevice_.lock(), shared_from_this() );

  // Create depth resources
  Image::create_depth_resources( depthImage, vulkanDevice_.lock(), swapChainExtent );
}

void VulkanSwapChain::clear_swap_chain() {
  // Clear image views
  swapChainImageViews.clear();

  // Clear swap chain
  swapChain = nullptr;
}

vk::SurfaceFormatKHR VulkanSwapChain::choose_swap_surface_format(
    const std::vector<vk::SurfaceFormatKHR> &availableFormats ) {
  // Logger::log( "Choosing swap format...", Logger::DEBUG );

  // Validate available formats
  assert( !availableFormats.empty() );

  // Find and return format
  const auto formatIt = std::ranges::find_if( availableFormats, []( const auto &format ) {
    return format.format == vk::Format::eB8G8R8A8Srgb &&
           format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
  } );

  // Return "best" (first) format
  return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR VulkanSwapChain::choose_swap_present_mode(
    const std::vector<vk::PresentModeKHR> &availablePresentModes ) {
  // VAlidate presentation mode is available
  assert( std::ranges::any_of( availablePresentModes, []( auto presentMode ) {
    // return Fifo, it should always be an option
    return presentMode == vk::PresentModeKHR::eFifo;
  } ) );

  // Find best presentation mode, we wanna use mailbox if its an option
  return std::ranges::any_of( availablePresentModes,
                              []( const vk::PresentModeKHR value ) {
                                return vk::PresentModeKHR::eMailbox == value;
                              } )
             ? vk::PresentModeKHR::eMailbox
             : vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanSwapChain::choose_swap_extent( const vk::SurfaceCapabilitiesKHR &capabilities ) {
  // Logger::log( "Choosing swap extent...", Logger::DEBUG );

  /**
   * What you are looking for is a part of you. remember what you've done.
   * Format: "Wh3n_I-wEN7 t2o ********_I_******-@-******!"
   */

  // Return current extent if new extent doesn't make sense
  if ( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() ) {
    return capabilities.currentExtent;
  }

  // Get screen width / hight
  int width, height;
  glfwGetFramebufferSize( window_, &width, &height );

  // Return clamped extent
  return { std::clamp<uint32_t>( width, capabilities.minImageExtent.width,
                                 capabilities.maxImageExtent.width ),
           std::clamp<uint32_t>( height, capabilities.minImageExtent.height,
                                 capabilities.maxImageExtent.height ) };
}

uint32_t VulkanSwapChain::choose_swap_min_image_count(
    vk::SurfaceCapabilitiesKHR const &surfaceCapabilities ) {
  // Get min count
  auto minImageCount = std::max( 3u, surfaceCapabilities.minImageCount );

  // Check for max count
  if ( ( 0 < surfaceCapabilities.maxImageCount ) &&
       ( surfaceCapabilities.maxImageCount < minImageCount ) ) {
    minImageCount = surfaceCapabilities.maxImageCount;
  }

  // Return image count
  return minImageCount;
}

void VulkanSwapChain::create_image_views() {
  // Logger::log( "Creating image views...", Logger::DEBUG );

  // Validate swap chain image views exist
  assert( swapChainImageViews.empty() );

  // Create image view info
  vk::ImageViewCreateInfo imageViewCreateInfo{
      .viewType = vk::ImageViewType::e2D,
      .format = swapChainSurfaceFormat.format,
      .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };

  // Added swap chain images
  for ( auto &image : swapChainImages ) {
    imageViewCreateInfo.image = image;
    swapChainImageViews.emplace_back( vulkanDevice_.lock()->device, imageViewCreateInfo );
  }
}

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
