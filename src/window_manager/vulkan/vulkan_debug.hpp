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

/**
 * @brief Debug message callback
 *
 * @param messageSeverity
 * @param messageType
 * @param pCallbackData
 * @param pUserData
 * @return VKAPI_ATTR
 */
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

/**
 * @brief Set up the debug messenger
 *
 * @param instance
 * @param debugMessenger
 */
void setup_debug_messenger(VkInstance instance,
                           VkDebugUtilsMessengerEXT *debugMessenger);

/**
 * @brief Creates utils
 *
 * @param instance
 * @param pCreateInfo
 * @param pAllocator
 * @param pDebugMessenger
 * @return VkResult
 */
VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

/**
 * @brief Destroy the utils
 *
 * @param instance
 * @param debugMessenger
 * @param pAllocator
 */
void destroy_debug_utils_messenger_ext(VkInstance instance,
                                       VkDebugUtilsMessengerEXT *debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);

/**
 * @brief Populate messenger info and set callback
 *
 * @param createInfo
 */
void populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo);

}; // namespace Debug
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
