/**
 * @file vulkan_command.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief vulkan_command cpp file
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vulkan_construct.hpp"

#include <vulkan/vulkan_core.h>

#include "logger.hpp"
#include "logger_helper.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan/vulkan_helper.hpp"
#include "vulkan/vulkan_initializers.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Construct {

void create_instance( VkInstance &instance ) {
  if ( enableValidationLayers && !check_validation_layer_support() ) {
    Logger::log( "validation layers requested, but not available!",
                 Logger::CRITICAL );
  }

  VkApplicationInfo appInfo = Initializer::application_info();

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto extensions = get_required_extensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>( extensions.size() );
  createInfo.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if ( enableValidationLayers ) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>( validationLayers.size() );
    createInfo.ppEnabledLayerNames = validationLayers.data();

    Debug::populate_debug_messenger_create_info( debugCreateInfo );
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;

    createInfo.pNext = nullptr;
  }

  if ( vkCreateInstance( &createInfo, nullptr, &instance ) != VK_SUCCESS ) {
    Logger::log( "Failed to create instance!", Logger::CRITICAL );
  }
}

void create_command_pool( VulkanDevice *vulkanDevice,
                          VkCommandPool &commandPool ) {
  QueueFamilyIndices queueFamilyIndices =
      vulkanDevice->find_queue_families( vulkanDevice->physicalDevice );

  VkCommandPoolCreateInfo poolInfo =
      Initializer::pool_info( queueFamilyIndices.graphicsFamily.value() );

  if ( vkCreateCommandPool( vulkanDevice->device, &poolInfo, nullptr,
                            &commandPool ) != VK_SUCCESS ) {
    Logger::log( "Failed to create command pool!", Logger::CRITICAL );
  }
}

void create_command_buffer( std::vector<VkCommandBuffer> &commandBuffers,
                            VkCommandPool commandPool, VkDevice device,
                            int maxFramesInFlight ) {
  commandBuffers.resize( maxFramesInFlight );
  VkCommandBufferAllocateInfo allocInfo =
      Initializer::command_buffer_allocate_info(
          commandPool, (uint32_t)commandBuffers.size() );

  if ( vkAllocateCommandBuffers( device, &allocInfo, commandBuffers.data() ) !=
       VK_SUCCESS ) {
    Logger::log( "Failed to allocate command buffers!", Logger::CRITICAL );
  }
}

}  // namespace Construct
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy