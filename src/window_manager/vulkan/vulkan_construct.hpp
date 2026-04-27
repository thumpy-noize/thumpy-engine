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
// #include <vulkan/vulkan_core.h>

#include <vulkan/vulkan_raii.hpp>

#include "vulkan/vulkan_helper.hpp"
#include "vulkan_device.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Construct {

// void instance( VkInstance &instance );

// TODO: look into context, we may not need to pass it in.
void raii_instance( vk::raii::Instance &instance, vk::raii::Context &context );

std::vector<const char *> getRequiredInstanceExtensions();

// void surface( VkInstance instance, GLFWwindow *window, VkSurfaceKHR &surface );

#pragma region Command pool

struct CommandPool {
  vk::raii::CommandPool pool = nullptr;
  vk::raii::CommandBuffer buffers = nullptr;

  // void destroy( VkDevice device ) { vkDestroyCommandPool( device, pool, nullptr ); }
};

void command_pool( vk::raii::CommandPool &commandPool, std::shared_ptr<VulkanDevice> vulkanDevice );

void command_buffer( std::shared_ptr<Construct::CommandPool> commandPool,
                     std::shared_ptr<VulkanDevice> vulkanDevice );

#pragma region Command pool

#pragma region Uniform buffers

// struct UniformBuffers {
//   std::vector<VkBuffer> buffers;
//   std::vector<VkDeviceMemory> memory;
//   std::vector<void *> mapped;
// };

// void uniform_buffers( VulkanDevice *vulkanDevice, UniformBuffers *uniformBuffers,
//                       int maxFramesInFlight );

#pragma endregion Uniform buffers

#pragma region Descriptor

// void descriptor_set_layout( VulkanDevice *vulkanDevice,
//                             VkDescriptorSetLayout &descriptorSetLayout );

// void descriptor_pool( VulkanDevice *vulkanDevice, VkDescriptorPool &descriptorPool,
//                       int maxFramesInFlight );

// void descriptor_sets( VulkanDevice *vulkanDevice, Descriptors *descriptors,
//                       std::vector<VkBuffer> &uniformBuffers, VulkanTextureImage *textureImage,
//                       int maxFramesInFlight );

#pragma endregion Descriptor

}  // namespace Construct
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
