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

#include "vulkan_device.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Construct {

void create_instance( VkInstance &instance );

void create_surface( VkInstance instance, GLFWwindow *window,
                     VkSurfaceKHR &surface );

void create_command_pool( VulkanDevice *vulkanDevice,
                          VkCommandPool &commandPool );

void create_command_buffer( std::vector<VkCommandBuffer> &commandBuffers,
                            VkCommandPool commandPool, VkDevice device,
                            int maxFramesInFlight );

void create_uniform_buffers( VulkanDevice *vulkanDevice,
                             std::vector<VkBuffer> &uniformBuffers,
                             std::vector<VkDeviceMemory> &uniformBuffersMemory,
                             std::vector<void *> &uniformBuffersMapped,
                             int maxFramesInFlight );

#pragma region Descriptor

void create_descriptor_set_layout( VulkanDevice *vulkanDevice,
                                   VkDescriptorSetLayout &descriptorSetLayout );

void create_descriptor_pool( VulkanDevice *vulkanDevice,
                             VkDescriptorPool &descriptorPool,
                             int maxFramesInFlight );

void create_descriptor_sets( VulkanDevice *vulkanDevice,
                             VkDescriptorSetLayout &descriptorSetLayout,
                             VkDescriptorPool &descriptorPool,
                             std::vector<VkDescriptorSet> &descriptorSets,
                             std::vector<VkBuffer> &uniformBuffers,
                             int maxFramesInFlight );

#pragma endregion Descriptor

}  // namespace Construct
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
