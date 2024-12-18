/**
 * @file vulkan_device.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief Vulkan device cpp file
 * @version 0.1
 * @date 2024-12-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vulkan_device.hpp"

#include <vulkan/vulkan_core.h>

#include <set>
#include <vector>

#include "vulkan/vulkan_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

VulkanDevice::VulkanDevice( VkInstance instance, VkSurfaceKHR surface ) {
  surface_ = surface;
  setup_device( instance );
}

void VulkanDevice::setup_device( VkInstance instance ) {
  pick_physical_device( instance );
  create_logical_device();
}

void VulkanDevice::pick_physical_device( VkInstance instance ) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices( instance, &deviceCount, nullptr );

  if ( deviceCount == 0 ) {
    throw VulkanNotCompatible( "Failed to find GPUs with Vulkan support!" );
  }

  std::vector<VkPhysicalDevice> devices( deviceCount );
  vkEnumeratePhysicalDevices( instance, &deviceCount, devices.data() );

  for ( const auto &device : devices ) {
    if ( is_device_suitable( device ) ) {
      physicalDevice = device;
      msaaSamples = get_max_usable_sample_count( physicalDevice );
      break;
    }
  }

  if ( physicalDevice == VK_NULL_HANDLE ) {
    throw VulkanNotCompatible( "Failed to find GPUs with Vulkan support!" );
  }
}

void VulkanDevice::create_logical_device() {
  QueueFamilyIndices indices = find_queue_families( physicalDevice );

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(),
                                             indices.presentFamily.value() };

  float queuePriority = 1.0f;
  for ( uint32_t queueFamily : uniqueQueueFamilies ) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back( queueCreateInfo );
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  deviceFeatures.samplerAnisotropy = VK_TRUE;
  deviceFeatures.sampleRateShading = VK_FALSE;

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.queueCreateInfoCount = static_cast<uint32_t>( queueCreateInfos.size() );
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = static_cast<uint32_t>( deviceExtensions.size() );
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if ( enableValidationLayers ) {
    createInfo.enabledLayerCount = static_cast<uint32_t>( validationLayers.size() );
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if ( vkCreateDevice( physicalDevice, &createInfo, nullptr, &device ) != VK_SUCCESS ) {
    throw VulkanNotCompatible( "Failed to create logical device!" );
  }

  vkGetDeviceQueue( device, indices.graphicsFamily.value(), 0, &graphicsQueue );
  vkGetDeviceQueue( device, indices.presentFamily.value(), 0, &presentQueue );
}

bool VulkanDevice::is_device_suitable( VkPhysicalDevice device ) {
  QueueFamilyIndices indices = find_queue_families( device );

  bool extensionsSupported = check_device_extension_support( device );

  bool swapChainAdequate = false;
  if ( extensionsSupported ) {
    SwapChainSupportDetails swapChainSupport = query_swap_chain_support( device );
    swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
  }

  VkPhysicalDeviceFeatures supportedFeatures;
  vkGetPhysicalDeviceFeatures( device, &supportedFeatures );

  return indices.is_complete() && extensionsSupported && swapChainAdequate &&
         supportedFeatures.samplerAnisotropy;
}

QueueFamilyIndices VulkanDevice::find_queue_families( VkPhysicalDevice device ) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

  std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
  vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.data() );

  int i = 0;
  for ( const auto &queueFamily : queueFamilies ) {
    if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface_, &presentSupport );

    if ( presentSupport ) {
      indices.presentFamily = i;
    }

    if ( indices.is_complete() ) {
      break;
    }

    i++;
  }

  return indices;
}

bool VulkanDevice::check_device_extension_support( VkPhysicalDevice device ) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );

  std::vector<VkExtensionProperties> availableExtensions( extensionCount );
  vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount,
                                        availableExtensions.data() );

  std::set<std::string> requiredExtensions( deviceExtensions.begin(), deviceExtensions.end() );

  for ( const auto &extension : availableExtensions ) {
    requiredExtensions.erase( extension.extensionName );
  }

  return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanDevice::query_swap_chain_support( VkPhysicalDevice device ) {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, surface_, &details.capabilities );

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface_, &formatCount, nullptr );

  if ( formatCount != 0 ) {
    details.formats.resize( formatCount );
    vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface_, &formatCount, details.formats.data() );
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface_, &presentModeCount, nullptr );

  if ( presentModeCount != 0 ) {
    details.presentModes.resize( presentModeCount );
    vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface_, &presentModeCount,
                                               details.presentModes.data() );
  }

  return details;
}

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
