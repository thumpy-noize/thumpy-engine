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

// VulkanDevice::VulkanDevice( VkInstance instance, VkSurfaceKHR surface ) {
//   surface_ = surface;
//   setup_device( instance );
// }

VulkanDevice::VulkanDevice( vk::raii::Instance &instance ) {
  Logger::log( "Constructing Vulkan device...", Logger::DEBUG );
  setup_device( instance );
}

void VulkanDevice::setup_device( vk::raii::Instance &instance ) {
  pick_physical_device( instance );
  create_logical_device();
}

void VulkanDevice::pick_physical_device( vk::raii::Instance &instance ) {
  Logger::log( "Picking physical device...", Logger::DEBUG );

  // Get physical devices
  std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
  auto const devIter = std::ranges::find_if( physicalDevices, [&]( auto const &physicalDevice ) {
    return is_device_suitable( physicalDevice );
  } );

  // Validate suitable gpu exist
  if ( devIter == physicalDevices.end() ) {
    throw std::runtime_error( "failed to find a suitable GPU!" );
  }

  // Set physical device
  physicalDevice = *devIter;

  // ### Deprecated ###
  // uint32_t deviceCount = 0;
  // vkEnumeratePhysicalDevices( instance, &deviceCount, nullptr );

  // if ( deviceCount == 0 ) {
  //   throw VulkanNotCompatible( "Failed to find GPUs with Vulkan support!" );
  // }

  // std::vector<VkPhysicalDevice> devices( deviceCount );
  // vkEnumeratePhysicalDevices( instance, &deviceCount, devices.data() );

  // for ( const auto &device : devices ) {
  //   if ( is_device_suitable( device ) ) {
  //     physicalDevice = device;
  //     msaaSamples = get_max_usable_sample_count( physicalDevice );
  //     break;
  //   }
  // }

  // if ( physicalDevice == VK_NULL_HANDLE ) {
  //   throw VulkanNotCompatible( "Failed to find GPUs with Vulkan support!" );
  // }
}

void VulkanDevice::create_logical_device() {
  Logger::log( "Creating logical device...", Logger::DEBUG );

  // Get queue family properties
  std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
      physicalDevice.getQueueFamilyProperties();

  // Get graphics queue family properties
  auto graphicsQueueFamilyProperty =
      std::ranges::find_if( queueFamilyProperties, []( auto const &qfp ) {
        return ( qfp.queueFlags & vk::QueueFlagBits::eGraphics ) !=
               static_cast<vk::QueueFlags>( 0 );
      } );

  // Validate properties
  if ( graphicsQueueFamilyProperty != queueFamilyProperties.end() ) {
    Logger::log( "No graphics queue family found!", Logger::ERROR_LOG );
  }

  assert( graphicsQueueFamilyProperty != queueFamilyProperties.end() &&
          "No graphics queue family found!" );

  // Get graphics index
  auto graphicsIndex = static_cast<uint32_t>(
      std::distance( queueFamilyProperties.begin(), graphicsQueueFamilyProperty ) );

  // Create structure chain
  vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
                     vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
      featureChain = {
          {},                               // vk::PhysicalDeviceFeatures2
          { .dynamicRendering = true },     // vk::PhysicalDeviceVulkan13Features
          { .extendedDynamicState = true }  // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
      };

  // Set queue priority (0-1 scale)
  float queuePriority = 0.5f;

  // Create device queue info
  vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
      .queueFamilyIndex = graphicsIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };

  // Create device info
  // enabledLayerCount / ppEnabledLayerNames is no longer required with updated implementation
  vk::DeviceCreateInfo deviceCreateInfo{
      .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &deviceQueueCreateInfo,
      .enabledExtensionCount = static_cast<uint32_t>( requiredDeviceExtension.size() ),
      .ppEnabledExtensionNames = requiredDeviceExtension.data() };

  // Create raii device
  device = vk::raii::Device( physicalDevice, deviceCreateInfo );

  // Create raii queue
  graphicsQueue = vk::raii::Queue( device, graphicsIndex, 0 );

  // ### Deprecated ###

  // QueueFamilyIndices indices = find_queue_families( physicalDevice );

  // std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  // std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(),
  //                                            indices.presentFamily.value() };

  // float queuePriority = 1.0f;
  // for ( uint32_t queueFamily : uniqueQueueFamilies ) {
  //   VkDeviceQueueCreateInfo queueCreateInfo{};
  //   queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  //   queueCreateInfo.queueFamilyIndex = queueFamily;
  //   queueCreateInfo.queueCount = 1;
  //   queueCreateInfo.pQueuePriorities = &queuePriority;
  //   queueCreateInfos.push_back( queueCreateInfo );
  // }

  // VkPhysicalDeviceFeatures deviceFeatures{};
  // deviceFeatures.samplerAnisotropy = VK_TRUE;
  // deviceFeatures.sampleRateShading = VK_FALSE;

  // VkDeviceCreateInfo createInfo{};
  // createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  // createInfo.queueCreateInfoCount = static_cast<uint32_t>( queueCreateInfos.size() );
  // createInfo.pQueueCreateInfos = queueCreateInfos.data();

  // createInfo.pEnabledFeatures = &deviceFeatures;

  // createInfo.enabledExtensionCount = static_cast<uint32_t>( deviceExtensions.size() );
  // createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  // if ( enableValidationLayers ) {
  //   createInfo.enabledLayerCount = static_cast<uint32_t>( validationLayers.size() );
  //   createInfo.ppEnabledLayerNames = validationLayers.data();
  // } else {
  //   createInfo.enabledLayerCount = 0;
  // }

  // if ( vkCreateDevice( physicalDevice, &createInfo, nullptr, &device ) != VK_SUCCESS ) {
  //   throw VulkanNotCompatible( "Failed to create logical device!" );
  // }

  // vkGetDeviceQueue( device, indices.graphicsFamily.value(), 0, &graphicsQueue );
  // vkGetDeviceQueue( device, indices.presentFamily.value(), 0, &presentQueue );
}

bool VulkanDevice::is_device_suitable( vk::raii::PhysicalDevice const &physicalDevice ) {
  // Check if the physicalDevice supports the Vulkan 1.3 API version (From vulkan guide)
  // We should probably update this to Vulkan1.4. But lets start here.
  bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

  // Check if any of the queue families support graphics operations
  auto queueFamilies = physicalDevice.getQueueFamilyProperties();
  bool supportsGraphics = std::ranges::any_of( queueFamilies, []( auto const &qfp ) {
    return !!( qfp.queueFlags & vk::QueueFlagBits::eGraphics );
  } );

  // Check if all required physicalDevice extensions are available
  auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
  bool supportsAllRequiredExtensions = std::ranges::all_of(
      requiredDeviceExtension, [&availableDeviceExtensions]( auto const &requiredDeviceExtension ) {
        return std::ranges::any_of(
            availableDeviceExtensions,
            [requiredDeviceExtension]( auto const &availableDeviceExtension ) {
              return strcmp( availableDeviceExtension.extensionName, requiredDeviceExtension ) == 0;
            } );
      } );

  // Check if the physicalDevice supports the required features
  auto features =
      physicalDevice
          .template getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
                                 vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
  bool supportsRequiredFeatures =
      features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
      features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
          .extendedDynamicState;

  // Return true if the physicalDevice meets all the criteria
  return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions &&
         supportsRequiredFeatures;

  // QueueFamilyIndices indices = find_queue_families( device );

  // bool extensionsSupported = check_device_extension_support( device );

  // bool swapChainAdequate = false;
  // if ( extensionsSupported ) {
  //   SwapChainSupportDetails swapChainSupport = query_swap_chain_support( device );
  //   swapChainAdequate = !swapChainSupport.formats.empty() &&
  //   !swapChainSupport.presentModes.empty();
  // }

  // VkPhysicalDeviceFeatures supportedFeatures;
  // vkGetPhysicalDeviceFeatures( device, &supportedFeatures );

  // return indices.is_complete() && extensionsSupported && swapChainAdequate &&
  //        supportedFeatures.samplerAnisotropy;
  return false;
}

// QueueFamilyIndices VulkanDevice::find_queue_families( VkPhysicalDevice device ) {
//   // QueueFamilyIndices indices;

//   // uint32_t queueFamilyCount = 0;
//   // vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

//   // std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
//   // vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.data() );

//   // int i = 0;
//   // for ( const auto &queueFamily : queueFamilies ) {
//   //   if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
//   //     indices.graphicsFamily = i;
//   //   }

//   //   VkBool32 presentSupport = false;
//   //   vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface_, &presentSupport );

//   //   if ( presentSupport ) {
//   //     indices.presentFamily = i;
//   //   }

//   //   if ( indices.is_complete() ) {
//   //     break;
//   //   }

//   //   i++;
//   // }

//   // return indices;
// }

// bool VulkanDevice::check_device_extension_support( VkPhysicalDevice device ) {
//   // uint32_t extensionCount;
//   // vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );

//   // std::vector<VkExtensionProperties> availableExtensions( extensionCount );
//   // vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount,
//   //                                       availableExtensions.data() );

//   // std::set<std::string> requiredExtensions( deviceExtensions.begin(), deviceExtensions.end()
//   );

//   // for ( const auto &extension : availableExtensions ) {
//   //   requiredExtensions.erase( extension.extensionName );
//   // }

//   // return requiredExtensions.empty();
//   return false;
// }

// SwapChainSupportDetails VulkanDevice::query_swap_chain_support( VkPhysicalDevice device ) {
//   SwapChainSupportDetails details;

//   // vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, surface_, &details.capabilities );

//   // uint32_t formatCount;
//   // vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface_, &formatCount, nullptr );

//   // if ( formatCount != 0 ) {
//   //   details.formats.resize( formatCount );
//   //   vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface_, &formatCount,
//   details.formats.data()
//   //   );
//   // }

//   // uint32_t presentModeCount;
//   // vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface_, &presentModeCount, nullptr );

//   // if ( presentModeCount != 0 ) {
//   //   details.presentModes.resize( presentModeCount );
//   //   vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface_, &presentModeCount,
//   //                                              details.presentModes.data() );
//   // }

//   return details;
// }

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
