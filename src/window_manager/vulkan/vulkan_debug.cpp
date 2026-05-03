#include "vulkan_debug.hpp"

#include <iostream>
#include <string>

#include "logger.hpp"
#include "vulkan_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Debug {

void setup_debug_messenger( vk::raii::Instance &instance,
                            vk::raii::DebugUtilsMessengerEXT &debugMessenger ) {
  // Check if using validation layers
  if ( !enableValidationLayers ) return;
  Logger::log( "Setting up Vulkan debug messenger...", Logger::DEBUG );

  // Set severity flags
  vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError );

  // Set message type flags
  vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation );

  // Create info
  vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
      .messageSeverity = severityFlags,
      .messageType = messageTypeFlags,
      .pfnUserCallback = &debug_callback };

  // Create debug messenger
  debugMessenger = instance.createDebugUtilsMessengerEXT( debugUtilsMessengerCreateInfoEXT );
}

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debug_callback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void * ) {
  // Set log level
  Logger::LogLevel level = Logger::NONE;

  switch ( severity ) {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
      level = Logger::ERROR_LOG;
      break;

    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
      level = Logger::WARNING;
      break;

    default:
      return vk::False;
  }

  // Log message
  Logger::log( "validation layer: type " + to_string( type ) + " msg: " + pCallbackData->pMessage,
               level );

  return vk::False;
}

}  // namespace Debug
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
