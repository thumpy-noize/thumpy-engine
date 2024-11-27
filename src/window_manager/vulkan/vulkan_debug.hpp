#pragma once

#include "logger.hpp"
#include "logger_helper.hpp"
#include <string>
#include <vulkan/vulkan_core.h>
namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Debug {

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
               VkDebugUtilsMessageTypeFlagsEXT messageType,
               const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
               void *pUserData) {
  std::string message = "validation layer: ";
  message.append(pCallbackData->pMessage);
  Logger::log(message, Logger::DEBUG);

  return VK_FALSE;
}

void setup_debug_messenger(VkInstance instance,
                           VkDebugUtilsMessengerEXT *debugMessenger);

VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

void destroy_debug_utils_messenger_ext(VkInstance instance,
                                       VkDebugUtilsMessengerEXT *debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);

void populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo);

void create_debug_utils_messenger_ext_dep(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

}; // namespace Debug
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
