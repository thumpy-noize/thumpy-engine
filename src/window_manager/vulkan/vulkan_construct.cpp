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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "logger.hpp"
#include "logger_helper.hpp"
#include "vulkan_buffers.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_initializers.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Construct {

void instance( VkInstance &instance ) {
  if ( enableValidationLayers && !check_validation_layer_support() ) {
    throw VulkanNotCompatible(
        "validation layers requested, but not available!" );
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
    throw VulkanNotCompatible( "Failed to create instance!" );
  }
}

void uniform_buffers( VulkanDevice *vulkanDevice,
                      std::vector<VkBuffer> &uniformBuffers,
                      std::vector<VkDeviceMemory> &uniformBuffersMemory,
                      std::vector<void *> &uniformBuffersMapped,
                      int maxFramesInFlight ) {
  VkDeviceSize bufferSize = sizeof( UniformBufferObject );

  uniformBuffers.resize( maxFramesInFlight );
  uniformBuffersMemory.resize( maxFramesInFlight );
  uniformBuffersMapped.resize( maxFramesInFlight );

  for ( size_t i = 0; i < maxFramesInFlight; i++ ) {
    Buffer::create_buffer( bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           uniformBuffers[i], uniformBuffersMemory[i],
                           vulkanDevice );

    vkMapMemory( vulkanDevice->device, uniformBuffersMemory[i], 0, bufferSize,
                 0, &uniformBuffersMapped[i] );
  }
}

void command_pool( VulkanDevice *vulkanDevice, VkCommandPool &commandPool ) {
  QueueFamilyIndices queueFamilyIndices =
      vulkanDevice->find_queue_families( vulkanDevice->physicalDevice );

  VkCommandPoolCreateInfo poolInfo =
      Initializer::pool_info( queueFamilyIndices.graphicsFamily.value() );

  if ( vkCreateCommandPool( vulkanDevice->device, &poolInfo, nullptr,
                            &commandPool ) != VK_SUCCESS ) {
    Logger::log( "Failed to create command pool!", Logger::CRITICAL );
  }
}

void command_buffer( std::vector<VkCommandBuffer> &commandBuffers,
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

void texture_image() {
  Logger::log( "Loading texture: " + get_texture_path() + "vj_swirl.png",
               Logger::WARNING );
  int texWidth, texHeight, texChannels;
  stbi_uc *pixels =
      stbi_load( std::string( get_texture_path() + "vj_swirl.png" ).c_str(),
                 &texWidth, &texHeight, &texChannels, STBI_rgb_alpha );
  VkDeviceSize imageSize = texWidth * texHeight * 4;

  if ( !pixels ) {
    Logger::log( "Failed to load texture image!", Logger::CRITICAL );
  }
}

#pragma region Descriptor

void descriptor_set_layout( VulkanDevice *vulkanDevice,
                            VkDescriptorSetLayout &descriptorSetLayout ) {
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount = 1;

  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding.pImmutableSamplers = nullptr;  // Optional

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &uboLayoutBinding;

  if ( vkCreateDescriptorSetLayout( vulkanDevice->device, &layoutInfo, nullptr,
                                    &descriptorSetLayout ) != VK_SUCCESS ) {
    Logger::log( "Failed to create descriptor set layout!", Logger::CRITICAL );
  }
}

void descriptor_pool( VulkanDevice *vulkanDevice,
                      VkDescriptorPool &descriptorPool,
                      int maxFramesInFlight ) {
  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = static_cast<uint32_t>( maxFramesInFlight );

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;

  poolInfo.maxSets = static_cast<uint32_t>( maxFramesInFlight );

  if ( vkCreateDescriptorPool( vulkanDevice->device, &poolInfo, nullptr,
                               &descriptorPool ) != VK_SUCCESS ) {
    Logger::log( "Failed to create descriptor pool!", Logger::CRITICAL );
  }
}

void descriptor_sets( VulkanDevice *vulkanDevice,
                      VkDescriptorSetLayout &descriptorSetLayout,
                      VkDescriptorPool &descriptorPool,
                      std::vector<VkDescriptorSet> &descriptorSets,
                      std::vector<VkBuffer> &uniformBuffers,
                      int maxFramesInFlight ) {
  std::vector<VkDescriptorSetLayout> layouts( maxFramesInFlight,
                                              descriptorSetLayout );
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>( maxFramesInFlight );
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets.resize( maxFramesInFlight );
  if ( vkAllocateDescriptorSets( vulkanDevice->device, &allocInfo,
                                 descriptorSets.data() ) != VK_SUCCESS ) {
    Logger::log( "Failed to allocate descriptor sets!", Logger::CRITICAL );
  }

  for ( size_t i = 0; i < maxFramesInFlight; i++ ) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof( UniformBufferObject );

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSets[i];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;        // Optional
    descriptorWrite.pTexelBufferView = nullptr;  // Optional

    vkUpdateDescriptorSets( vulkanDevice->device, 1, &descriptorWrite, 0,
                            nullptr );
  }
}

#pragma endregion Descriptor

}  // namespace Construct
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy