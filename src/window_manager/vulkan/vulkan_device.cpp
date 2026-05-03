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

VulkanDevice::VulkanDevice( vk::raii::Instance &instance, vk::raii::SurfaceKHR &surface ) {
  Logger::log( "Constructing Vulkan device...", Logger::DEBUG );
  setup_device( instance, surface );
}

void VulkanDevice::setup_device( vk::raii::Instance &instance, vk::raii::SurfaceKHR &surface ) {
  // Pick physical device
  pick_physical_device( instance );
  // Get max sample count
  msaaSamples = get_max_usable_sample_count();
  // Create logical device
  create_logical_device( surface );
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
    throw VulkanNotCompatible( "Failed to find a suitable GPU!" );
  }

  // Set physical device
  physicalDevice = *devIter;
}

void VulkanDevice::create_logical_device( vk::raii::SurfaceKHR &surface ) {
  Logger::log( "Creating logical device...", Logger::DEBUG );

  // Get queue family properties
  std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
      physicalDevice.getQueueFamilyProperties();

  // Get the first index into queueFamilyProperties which supports both graphics and present
  for ( uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++ ) {
    if ( ( queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics ) &&
         physicalDevice.getSurfaceSupportKHR( qfpIndex, *surface ) ) {
      // Found a queue family that supports both graphics and present
      queueIndex = qfpIndex;
      break;
    }
  }
  if ( queueIndex == ~0 ) {
    throw VulkanNotCompatible( "Could not find a queue for graphics and present -> terminating" );
  }

  // Create structure chain
  vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                     vk::PhysicalDeviceVulkan13Features,
                     vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
      featureChain = {
          { .features = { .samplerAnisotropy = true } },  // vk::PhysicalDeviceFeatures2
          { .shaderDrawParameters = true },               // vk::PhysicalDeviceVulkan11Features
          { .synchronization2 = true,
            .dynamicRendering = true },     // vk::PhysicalDeviceVulkan13Features
          { .extendedDynamicState = true }  // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
      };

  // Set queue priority (0-1 scale)
  float queuePriority = 0.5f;

  // Create device queue info
  vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
      .queueFamilyIndex = queueIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };

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
  graphicsQueue = vk::raii::Queue( device, queueIndex, 0 );
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
  auto features = physicalDevice.template getFeatures2<
      vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
      vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

  bool supportsRequiredFeatures =
      features.template get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy &&
      features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
      features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
      features.template get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
      features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
          .extendedDynamicState;

  // Return true if the physicalDevice meets all the criteria
  return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions &&
         supportsRequiredFeatures;
}

vk::SampleCountFlagBits VulkanDevice::get_max_usable_sample_count() {
  // Get physical device properties
  vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();

  // Set device flags
  vk::SampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                physicalDeviceProperties.limits.framebufferDepthSampleCounts;

  // Return max sample count // TODO: Look into converting this to a switch statement
  if ( counts & vk::SampleCountFlagBits::e64 ) {
    return vk::SampleCountFlagBits::e64;
  }
  if ( counts & vk::SampleCountFlagBits::e32 ) {
    return vk::SampleCountFlagBits::e32;
  }
  if ( counts & vk::SampleCountFlagBits::e16 ) {
    return vk::SampleCountFlagBits::e16;
  }
  if ( counts & vk::SampleCountFlagBits::e8 ) {
    return vk::SampleCountFlagBits::e8;
  }
  if ( counts & vk::SampleCountFlagBits::e4 ) {
    return vk::SampleCountFlagBits::e4;
  }
  if ( counts & vk::SampleCountFlagBits::e2 ) {
    return vk::SampleCountFlagBits::e2;
  }

  // Return default
  return vk::SampleCountFlagBits::e1;
}

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
