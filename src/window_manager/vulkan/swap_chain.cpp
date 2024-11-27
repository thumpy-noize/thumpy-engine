#include "swap_chain.hpp"
#include <algorithm> // Necessary for std::clamp
#include <limits>
#include <stdexcept>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

SwapChain::SwapChain(VkInstance instance, VkPhysicalDevice physicalDevice,
                     VkDevice device, VkSurfaceKHR surface,
                     GLFWwindow *window) {
  set_context(instance, physicalDevice, device, surface, window);
  create_swap_chain();
}

void SwapChain::create_swap_chain() {
  SwapChainSupportDetails swapChainSupport =
      query_swap_chain_support(physicalDevice_);

  VkSurfaceFormatKHR surfaceFormat =
      choose_swap_surface_format(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      choose_swap_present_mode(swapChainSupport.presentModes);
  VkExtent2D extent = choose_swap_extent(swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface_;

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = find_queue_families(physicalDevice_);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
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

  // if (vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swapChain_) !=
  //     VK_SUCCESS) {
  //   throw std::runtime_error("failed to create swap chain!");
  // }

  //   vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, nullptr);
  //   swapChainImages_.resize(imageCount);
  //   vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount,
  //                           swapChainImages_.data());

  //   swapChainImageFormat_ = surfaceFormat.format;
  //   swapChainExtent_ = extent;
}

void SwapChain::recreate_swap_chain() {
  //   Logger::log("Recreating swap chain...", Logger::INFO);
  //   int width = 0, height = 0;
  //   glfwGetFramebufferSize(window_, &width, &height);
  //   while (width == 0 || height == 0) {
  //     glfwGetFramebufferSize(window_, &width, &height);
  //     glfwWaitEvents();
  // }
  //   vkDeviceWaitIdle(device_);

  //   clear_swap_chain();

  //   create_swap_chain();
  //   create_image_views();
  //   create_framebuffers();
}

void SwapChain::clear_swap_chain() {
  //   for (auto framebuffer : swapChainFramebuffers_) {
  //     vkDestroyFramebuffer(device_, framebuffer, nullptr);
  //   }

  //   for (auto imageView : swapChainImageViews_) {
  //     vkDestroyImageView(device_, imageView, nullptr);
  //   }

  //   vkDestroySwapchainKHR(device_, swapChain_, nullptr);
}

void SwapChain::set_context(VkInstance instance,
                            VkPhysicalDevice physicalDevice, VkDevice device,
                            VkSurfaceKHR surface, GLFWwindow *window) {
  this->instance_ = instance;
  this->physicalDevice_ = physicalDevice;
  this->device_ = device;
  this->surface_ = surface;
  this->window_ = window;
}

SwapChainSupportDetails
SwapChain::query_swap_chain_support(VkPhysicalDevice device) {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_,
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount,
                                            nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface_, &presentModeCount, details.presentModes.data());
  }

  return details;
}

VkSurfaceFormatKHR SwapChain::choose_swap_surface_format(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR SwapChain::choose_swap_present_mode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
SwapChain::choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    std::clamp(actualExtent.width, capabilities.minImageExtent.width,
               capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

QueueFamilyIndices SwapChain::find_queue_families(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);

    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.is_complete()) {
      break;
    }

    i++;
  }

  return indices;
}

} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
