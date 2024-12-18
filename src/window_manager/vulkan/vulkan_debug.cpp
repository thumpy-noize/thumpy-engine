#include "vulkan_debug.hpp"

#include <string>

#include "logger.hpp"
#include "vulkan_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Debug {

void setup_debug_messenger( VkInstance instance,
                            VkDebugUtilsMessengerEXT *debugMessenger ) {
  if ( !enableValidationLayers ) return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  populate_debug_messenger_create_info( createInfo );
  if ( create_debug_utils_messenger_ext( instance, &createInfo, nullptr,
                                         debugMessenger ) != VK_SUCCESS ) {
    Logger::log( "Failed to set up debug messenger!", Logger::CRITICAL );
  }
}

VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger ) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT" );
  if ( func != nullptr ) {
    return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void destroy_debug_utils_messenger_ext(
    VkInstance instance, VkDebugUtilsMessengerEXT *debugMessenger,
    const VkAllocationCallbacks *pAllocator ) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT" );
  if ( func != nullptr ) {
    func( instance, *debugMessenger, pAllocator );
  }
}

void populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo ) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debug_callback;
}

}  // namespace Debug
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
