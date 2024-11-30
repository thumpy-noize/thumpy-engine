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

#include "vulkan_command.hpp"

#include <vulkan/vulkan_core.h>

#include "logger.hpp"
#include "logger_helper.hpp"
#include "vulkan/vulkan_helper.hpp"
#include "vulkan/vulkan_initializers.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_swap_chain.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

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
                            int max_frames_in_flight ) {
  commandBuffers.resize( max_frames_in_flight );
  VkCommandBufferAllocateInfo allocInfo =
      Initializer::command_buffer_allocate_info(
          commandPool, (uint32_t)commandBuffers.size() );

  if ( vkAllocateCommandBuffers( device, &allocInfo, commandBuffers.data() ) !=
       VK_SUCCESS ) {
    Logger::log( "Failed to allocate command buffers!", Logger::CRITICAL );
  }
}

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
