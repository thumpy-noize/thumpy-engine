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

#include <vulkan/vulkan_raii.hpp>

#include "vulkan/vulkan_helper.hpp"
#include "vulkan/vulkan_image.hpp"
#include "vulkan_device.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Construct {

// TODO: look into context, we may not need to pass it in.
void instance( vk::raii::Instance &instance, vk::raii::Context &context );

std::vector<const char *> getRequiredInstanceExtensions();

#pragma region Command pool

struct CommandPool {
  vk::raii::CommandPool pool = nullptr;
  std::vector<vk::raii::CommandBuffer> buffers;
  std::vector<vk::raii::CommandBuffer> computeBuffers;  // TODO: used for compute shaders
};

void command_pool( vk::raii::CommandPool &commandPool, std::shared_ptr<VulkanDevice> vulkanDevice );

void command_buffer( std::shared_ptr<Construct::CommandPool> commandPool,
                     std::shared_ptr<VulkanDevice> vulkanDevice, uint32_t maxFramesInFlight );

#pragma region Command pool

#pragma region Descriptor

void descriptor_set_layout( std::shared_ptr<VulkanDevice> vulkanDevice,
                            vk::raii::DescriptorSetLayout &descriptorSetLayout );

void descriptor_pool( std::shared_ptr<VulkanDevice> vulkanDevice,
                      vk::raii::DescriptorPool &descriptorPool, uint32_t maxFramesInFlight );

void descriptor_sets( std::shared_ptr<VulkanDevice> vulkanDevice,
                      std::shared_ptr<Descriptors> descriptors,
                      std::vector<vk::raii::Buffer> &uniformBuffers,
                      std::shared_ptr<Image::VulkanTextureImage> vulkanTextureImage,
                      uint32_t maxFramesInFlight );

#pragma endregion Descriptor

}  // namespace Construct
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
