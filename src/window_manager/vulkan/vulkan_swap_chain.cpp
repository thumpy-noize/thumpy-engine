#include "vulkan_swap_chain.hpp"

#include <algorithm>  // Necessary for std::clamp
#include <limits>

#include "logger.hpp"
#include "vulkan/vulkan_image.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

VulkanSwapChain::VulkanSwapChain( VulkanDevice *vulkanDevice,
                                  GLFWwindow *window, VkSurfaceKHR surface ) {
  vulkanDevice_ = vulkanDevice;
  surface_ = surface;
  window_ = window;
  create_swap_chain();
  create_image_views();
  create_render_pass();
}

void VulkanSwapChain::create_swap_chain() {
  SwapChainSupportDetails swapChainSupport = query_swap_chain_support();

  VkSurfaceFormatKHR surfaceFormat =
      choose_swap_surface_format( swapChainSupport.formats );
  VkPresentModeKHR presentMode =
      choose_swap_present_mode( swapChainSupport.presentModes );
  VkExtent2D chosen_extent =
      choose_swap_extent( swapChainSupport.capabilities );

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if ( swapChainSupport.capabilities.maxImageCount > 0 &&
       imageCount > swapChainSupport.capabilities.maxImageCount ) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface_;

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = chosen_extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices =
      vulkanDevice_->find_queue_families( vulkanDevice_->physicalDevice );
  uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(),
                                    indices.presentFamily.value() };

  if ( indices.graphicsFamily != indices.presentFamily ) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if ( vkCreateSwapchainKHR( vulkanDevice_->device, &createInfo, nullptr,
                             &swapChain ) != VK_SUCCESS ) {
    Logger::log( "failed to create swap chain!", Logger::CRITICAL );
  }

  vkGetSwapchainImagesKHR( vulkanDevice_->device, swapChain, &imageCount,
                           nullptr );
  swapChainImages_.resize( imageCount );
  vkGetSwapchainImagesKHR( vulkanDevice_->device, swapChain, &imageCount,
                           swapChainImages_.data() );

  swapChainImageFormat = surfaceFormat.format;
  extent = chosen_extent;
}

void VulkanSwapChain::recreate_swap_chain() {
  Logger::log( "Recreating swap chain...", Logger::INFO );
  int width = 0, height = 0;
  glfwGetFramebufferSize( window_, &width, &height );
  while ( width == 0 || height == 0 ) {
    glfwGetFramebufferSize( window_, &width, &height );
    glfwWaitEvents();
  }

  vkDeviceWaitIdle( vulkanDevice_->device );

  clear_swap_chain();

  create_swap_chain();
  create_image_views();
  create_framebuffers();
}

void VulkanSwapChain::clear_swap_chain() {
  for ( auto framebuffer : swapChainFramebuffers ) {
    vkDestroyFramebuffer( vulkanDevice_->device, framebuffer, nullptr );
  }

  for ( auto imageView : swapChainImageViews ) {
    vkDestroyImageView( vulkanDevice_->device, imageView, nullptr );
  }

  vkDestroySwapchainKHR( vulkanDevice_->device, swapChain, nullptr );
}

SwapChainSupportDetails VulkanSwapChain::query_swap_chain_support() {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR( vulkanDevice_->physicalDevice,
                                             surface_, &details.capabilities );

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR( vulkanDevice_->physicalDevice, surface_,
                                        &formatCount, nullptr );

  if ( formatCount != 0 ) {
    details.formats.resize( formatCount );
    vkGetPhysicalDeviceSurfaceFormatsKHR( vulkanDevice_->physicalDevice,
                                          surface_, &formatCount,
                                          details.formats.data() );
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      vulkanDevice_->physicalDevice, surface_, &presentModeCount, nullptr );

  if ( presentModeCount != 0 ) {
    details.presentModes.resize( presentModeCount );
    vkGetPhysicalDeviceSurfacePresentModesKHR( vulkanDevice_->physicalDevice,
                                               surface_, &presentModeCount,
                                               details.presentModes.data() );
  }
  return details;
}

VkSurfaceFormatKHR VulkanSwapChain::choose_swap_surface_format(
    const std::vector<VkSurfaceFormatKHR> &availableFormats ) {
  for ( const auto &availableFormat : availableFormats ) {
    if ( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
         availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::choose_swap_present_mode(
    const std::vector<VkPresentModeKHR> &availablePresentModes ) {
  for ( const auto &availablePresentMode : availablePresentModes ) {
    if ( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR ) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::choose_swap_extent(
    const VkSurfaceCapabilitiesKHR &capabilities ) {
  if ( capabilities.currentExtent.width !=
       std::numeric_limits<uint32_t>::max() ) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize( window_, &width, &height );

    VkExtent2D actualExtent = { static_cast<uint32_t>( width ),
                                static_cast<uint32_t>( height ) };

    actualExtent.width =
        std::clamp( actualExtent.width, capabilities.minImageExtent.width,
                    capabilities.maxImageExtent.width );
    actualExtent.height =
        std::clamp( actualExtent.height, capabilities.minImageExtent.height,
                    capabilities.maxImageExtent.height );

    return actualExtent;
  }
}

void VulkanSwapChain::create_image_views() {
  // resize image views
  swapChainImageViews.resize( swapChainImages_.size() );

  // iterate over swap chain images
  for ( size_t i = 0; i < swapChainImages_.size(); i++ ) {
    swapChainImageViews[i] = Image::create_image_view(
        vulkanDevice_->device, swapChainImages_[i], swapChainImageFormat );
  }
}

void VulkanSwapChain::create_framebuffers() {
  swapChainFramebuffers.resize( swapChainImageViews.size() );

  for ( size_t i = 0; i < swapChainImageViews.size(); i++ ) {
    VkImageView attachments[] = { swapChainImageViews[i] };

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if ( vkCreateFramebuffer( vulkanDevice_->device, &framebufferInfo, nullptr,
                              &swapChainFramebuffers[i] ) != VK_SUCCESS ) {
      Logger::log( "failed to create framebuffer!", Logger::CRITICAL );
    }
  }
}

void VulkanSwapChain::create_render_pass() {
  // ### color attachment ###
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // ### subpass ###
  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  // ### render pass ###
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if ( vkCreateRenderPass( vulkanDevice_->device, &renderPassInfo, nullptr,
                           &renderPass ) != VK_SUCCESS ) {
    Logger::log( "failed to create render pass!", Logger::CRITICAL );
  }
}

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
