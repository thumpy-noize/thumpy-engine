/**
 * @file vulkan_command.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief This scrip handles Vulkan command pool & buffers
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "vulkan/vulkan_helper.hpp"
#include "vulkan_device.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Construct {

void instance( VkInstance &instance );

void surface( VkInstance instance, GLFWwindow *window, VkSurfaceKHR &surface );

#pragma region Command pool

struct CommandPool {
  VkCommandPool pool;
  std::vector<VkCommandBuffer> buffers;

  void destroy( VkDevice device ) { vkDestroyCommandPool( device, pool, nullptr ); }
};

void command_pool( VulkanDevice *vulkanDevice, VkCommandPool &commandPool );

void command_buffer( std::vector<VkCommandBuffer> &commandBuffers, VkCommandPool commandPool,
                     VkDevice device, int maxFramesInFlight );

#pragma region Command pool

#pragma region Uniform buffers

struct UniformBuffers {
  std::vector<VkBuffer> buffers;
  std::vector<VkDeviceMemory> memory;
  std::vector<void *> mapped;
};

void uniform_buffers( VulkanDevice *vulkanDevice, UniformBuffers *uniformBuffers,
                      int maxFramesInFlight );

#pragma endregion Uniform buffers

#pragma region Descriptor

void descriptor_set_layout( VulkanDevice *vulkanDevice,
                            VkDescriptorSetLayout &descriptorSetLayout );

void descriptor_pool( VulkanDevice *vulkanDevice, VkDescriptorPool &descriptorPool,
                      int maxFramesInFlight );

void descriptor_sets( VulkanDevice *vulkanDevice, Descriptors *descriptors,
                      std::vector<VkBuffer> &uniformBuffers, VulkanTextureImage *textureImage,
                      int maxFramesInFlight );

#pragma endregion Descriptor

}  // namespace Construct
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
