/**
 * @file vulkan_debug.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Contains methods for Vulkan debug messenger.
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <vulkan/vulkan_core.h>

#include <string>
#include <vulkan/vulkan_raii.hpp>

#include "logger.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Debug {

/**
 * @brief setup the RAII debug messenger
 *
 */
void setup_debug_messenger( vk::raii::Instance &instance,
                            vk::raii::DebugUtilsMessengerEXT &debugMessenger );

/**
 * @brief RAII callback method
 *
 */
static VKAPI_ATTR vk::Bool32 VKAPI_CALL debug_callback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void * );

};  // namespace Debug
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
