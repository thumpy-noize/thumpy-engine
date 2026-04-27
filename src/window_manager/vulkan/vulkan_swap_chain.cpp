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
// #include "vulkan_image.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

VulkanSwapChain::VulkanSwapChain( std::shared_ptr<VulkanDevice> vulkanDevice, GLFWwindow *window,
                                  vk::raii::SurfaceKHR &surface ) {
  Logger::log( "Constructing Vulkan swap chain...", Logger::DEBUG );

  // Get weak ptr to vulkan device
  vulkanDevice_ = vulkanDevice;

  // Get ptr to window // TODO: make weak ptr
  window_ = window;

  // Create swap chain
  create_swap_chain( surface );

  // Create image views
  create_image_views();

  // surface_ = surface;
  // create_swap_chain();
  // create_image_views();
  // create_render_pass();
}

void VulkanSwapChain::create_swap_chain( vk::raii::SurfaceKHR &surface ) {
  Logger::log( "Creating swap chain...", Logger::DEBUG );

  // Get surface capabilities
  vk::SurfaceCapabilitiesKHR surfaceCapabilities =
      vulkanDevice_.lock()->physicalDevice.getSurfaceCapabilitiesKHR( *surface );

  // Get extent
  swapChainExtent = choose_swap_extent( surfaceCapabilities );

  // Get min image count
  uint32_t minImageCount = choose_swap_min_image_count( surfaceCapabilities );

  // Get surface format
  std::vector<vk::SurfaceFormatKHR> availableFormats =
      vulkanDevice_.lock()->physicalDevice.getSurfaceFormatsKHR( *surface );
  swapChainSurfaceFormat = choose_swap_surface_format( availableFormats );

  // Get present mode
  std::vector<vk::PresentModeKHR> availablePresentModes =
      vulkanDevice_.lock()->physicalDevice.getSurfacePresentModesKHR( *surface );
  vk::PresentModeKHR presentMode = choose_swap_present_mode( availablePresentModes );

  vk::SwapchainCreateInfoKHR swapChainCreateInfo{
      .surface = *surface,
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

  //   SwapChainSupportDetails swapChainSupport = query_swap_chain_support();

  //   VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format( swapChainSupport.formats );
  //   VkPresentModeKHR presentMode = choose_swap_present_mode( swapChainSupport.presentModes );
  //   VkExtent2D chosen_extent = choose_swap_extent( swapChainSupport.capabilities );

  //   uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  //   if ( swapChainSupport.capabilities.maxImageCount > 0 &&
  //        imageCount > swapChainSupport.capabilities.maxImageCount ) {
  //     imageCount = swapChainSupport.capabilities.maxImageCount;
  //   }

  //   VkSwapchainCreateInfoKHR createInfo{};
  //   createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  //   createInfo.surface = surface_;

  //   createInfo.minImageCount = imageCount;
  //   createInfo.imageFormat = surfaceFormat.format;
  //   createInfo.imageColorSpace = surfaceFormat.colorSpace;
  //   createInfo.imageExtent = chosen_extent;
  //   createInfo.imageArrayLayers = 1;
  //   createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  //   QueueFamilyIndices indices = vulkanDevice_->find_queue_families(
  //   vulkanDevice_->physicalDevice
  //   ); uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(),
  //   indices.presentFamily.value() };

  //   if ( indices.graphicsFamily != indices.presentFamily ) {
  //     createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
  //     createInfo.queueFamilyIndexCount = 2;
  //     createInfo.pQueueFamilyIndices = queueFamilyIndices;
  //   } else {
  //     createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //   }

  //   createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  //   createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  //   createInfo.presentMode = presentMode;
  //   createInfo.clipped = VK_TRUE;

  //   createInfo.oldSwapchain = VK_NULL_HANDLE;

  //   if ( vkCreateSwapchainKHR( vulkanDevice_->device, &createInfo, nullptr, &swapChain ) !=
  //        VK_SUCCESS ) {
  //     Logger::log( "failed to create swap chain!", Logger::CRITICAL );
  //   }

  //   vkGetSwapchainImagesKHR( vulkanDevice_->device, swapChain, &imageCount, nullptr );
  //   swapChainImages_.resize( imageCount );
  //   vkGetSwapchainImagesKHR( vulkanDevice_->device, swapChain, &imageCount,
  //   swapChainImages_.data()
  //   );

  //   swapChainImageFormat = surfaceFormat.format;
  //   extent = chosen_extent;
}

// void VulkanSwapChain::recreate_swap_chain( VulkanImage *depthImage, VulkanImage *colorImage ) {
//   Logger::log( "Recreating swap chain...", Logger::INFO );
//   int width = 0, height = 0;
//   glfwGetFramebufferSize( window_, &width, &height );
//   while ( width == 0 || height == 0 ) {
//     glfwGetFramebufferSize( window_, &width, &height );
//     glfwWaitEvents();
//   }

//   vkDeviceWaitIdle( vulkanDevice_->device );

//   clear_swap_chain();
//   depthImage->destroy( vulkanDevice_->device );
//   colorImage->destroy( vulkanDevice_->device );

//   create_swap_chain();
//   create_image_views();
//   Image::create_color_resources( colorImage, vulkanDevice_, this );
//   Image::create_depth_resources( depthImage, vulkanDevice_, extent );
//   Buffer::create_framebuffers( this, depthImage->imageView, colorImage->imageView,
//                                vulkanDevice_->device );
// }

// void VulkanSwapChain::clear_swap_chain() {
//   for ( auto framebuffer : swapChainFramebuffers ) {
//     vkDestroyFramebuffer( vulkanDevice_->device, framebuffer, nullptr );
//   }

//   for ( auto imageView : swapChainImageViews ) {
//     vkDestroyImageView( vulkanDevice_->device, imageView, nullptr );
//   }

//   vkDestroySwapchainKHR( vulkanDevice_->device, swapChain, nullptr );
// }

// SwapChainSupportDetails VulkanSwapChain::query_swap_chain_support() {
//   SwapChainSupportDetails details;

//   vkGetPhysicalDeviceSurfaceCapabilitiesKHR( vulkanDevice_->physicalDevice, surface_,
//                                              &details.capabilities );

//   uint32_t formatCount;
//   vkGetPhysicalDeviceSurfaceFormatsKHR( vulkanDevice_->physicalDevice, surface_, &formatCount,
//                                         nullptr );

//   if ( formatCount != 0 ) {
//     details.formats.resize( formatCount );
//     vkGetPhysicalDeviceSurfaceFormatsKHR( vulkanDevice_->physicalDevice, surface_,
//     &formatCount,
//                                           details.formats.data() );
//   }

//   uint32_t presentModeCount;
//   vkGetPhysicalDeviceSurfacePresentModesKHR( vulkanDevice_->physicalDevice, surface_,
//                                              &presentModeCount, nullptr );

//   if ( presentModeCount != 0 ) {
//     details.presentModes.resize( presentModeCount );
//     vkGetPhysicalDeviceSurfacePresentModesKHR( vulkanDevice_->physicalDevice, surface_,
//                                                &presentModeCount, details.presentModes.data()
//                                                );
//   }
//   return details;
// }

vk::SurfaceFormatKHR VulkanSwapChain::choose_swap_surface_format(
    const std::vector<vk::SurfaceFormatKHR> &availableFormats ) {
  Logger::log( "Choosing swap format...", Logger::DEBUG );

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
  Logger::log( "Choosing swap extent...", Logger::DEBUG );

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
  Logger::log( "Creating image views...", Logger::DEBUG );

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

// void VulkanSwapChain::create_framebuffers() {
//   swapChainFramebuffers.resize( swapChainImageViews.size() );

//   for ( size_t i = 0; i < swapChainImageViews.size(); i++ ) {
//     VkImageView attachments[] = { swapChainImageViews[i] };

//     VkFramebufferCreateInfo framebufferInfo{};
//     framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//     framebufferInfo.renderPass = renderPass;
//     framebufferInfo.attachmentCount = 1;
//     framebufferInfo.pAttachments = attachments;
//     framebufferInfo.width = extent.width;
//     framebufferInfo.height = extent.height;
//     framebufferInfo.layers = 1;

//     if ( vkCreateFramebuffer( vulkanDevice_->device, &framebufferInfo, nullptr,
//                               &swapChainFramebuffers[i] ) != VK_SUCCESS ) {
//       Logger::log( "failed to create framebuffer!", Logger::CRITICAL );
//     }
//   }
// }

// void VulkanSwapChain::create_render_pass() {
// ### color ###
// VkAttachmentDescription colorAttachment{};
// colorAttachment.format = swapChainImageFormat;
// colorAttachment.samples = vulkanDevice_->msaaSamples;
// colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
// colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
// colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
// colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
// colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
// colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

// VkAttachmentReference colorAttachmentRef{};
// colorAttachmentRef.attachment = 0;
// colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

// // ### depth buffer ###
// VkAttachmentDescription depthAttachment{};
// depthAttachment.format = Image::find_depth_format( vulkanDevice_->physicalDevice );
// depthAttachment.samples = vulkanDevice_->msaaSamples;
// depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
// depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
// depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
// depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
// depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
// depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

// VkAttachmentReference depthAttachmentRef{};
// depthAttachmentRef.attachment = 1;
// depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

// // ### msaa buffer ###
// VkAttachmentDescription colorAttachmentResolve{};
// colorAttachmentResolve.format = swapChainImageFormat;
// colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
// colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
// colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
// colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
// colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
// colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
// colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

// VkAttachmentReference colorAttachmentResolveRef{};
// colorAttachmentResolveRef.attachment = 2;
// colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

// // ### subpass ###
// VkSubpassDescription subpass{};
// subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
// subpass.colorAttachmentCount = 1;
// subpass.pColorAttachments = &colorAttachmentRef;
// subpass.pDepthStencilAttachment = &depthAttachmentRef;
// subpass.pResolveAttachments = &colorAttachmentResolveRef;

// VkSubpassDependency dependency{};
// dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
// dependency.dstSubpass = 0;
// dependency.srcStageMask =
//     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
//     VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
// dependency.srcAccessMask =
//     VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
// dependency.dstStageMask =
//     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
//     VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
// dependency.dstAccessMask =
//     VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

// // ### render pass ###
// std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment,
//                                                        colorAttachmentResolve };
// VkRenderPassCreateInfo renderPassInfo{};
// renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
// renderPassInfo.attachmentCount = static_cast<uint32_t>( attachments.size() );
// renderPassInfo.pAttachments = attachments.data();
// renderPassInfo.subpassCount = 1;
// renderPassInfo.pSubpasses = &subpass;
// renderPassInfo.dependencyCount = 1;
// renderPassInfo.pDependencies = &dependency;

// if ( vkCreateRenderPass( vulkanDevice_->device, &renderPassInfo, nullptr, &renderPass ) !=
//      VK_SUCCESS ) {
//   Logger::log( "failed to create render pass!", Logger::CRITICAL );
// }
// }

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
