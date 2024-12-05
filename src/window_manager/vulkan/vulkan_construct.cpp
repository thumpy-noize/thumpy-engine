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
    throw VulkanNotCompatible( "validation layers requested, but not available!" );
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
    createInfo.enabledLayerCount = static_cast<uint32_t>( validationLayers.size() );
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

void uniform_buffers( VulkanDevice *vulkanDevice, std::vector<VkBuffer> &uniformBuffers,
                      std::vector<VkDeviceMemory> &uniformBuffersMemory,
                      std::vector<void *> &uniformBuffersMapped, int maxFramesInFlight ) {
  VkDeviceSize bufferSize = sizeof( UniformBufferObject );

  uniformBuffers.resize( maxFramesInFlight );
  uniformBuffersMemory.resize( maxFramesInFlight );
  uniformBuffersMapped.resize( maxFramesInFlight );

  for ( size_t i = 0; i < maxFramesInFlight; i++ ) {
    Buffer::create_buffer(
        bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        uniformBuffers[i], uniformBuffersMemory[i], vulkanDevice );

    vkMapMemory( vulkanDevice->device, uniformBuffersMemory[i], 0, bufferSize, 0,
                 &uniformBuffersMapped[i] );
  }
}

void command_pool( VulkanDevice *vulkanDevice, VkCommandPool &commandPool ) {
  QueueFamilyIndices queueFamilyIndices =
      vulkanDevice->find_queue_families( vulkanDevice->physicalDevice );

  VkCommandPoolCreateInfo poolInfo =
      Initializer::pool_info( queueFamilyIndices.graphicsFamily.value() );

  if ( vkCreateCommandPool( vulkanDevice->device, &poolInfo, nullptr, &commandPool ) !=
       VK_SUCCESS ) {
    Logger::log( "Failed to create command pool!", Logger::CRITICAL );
  }
}

void command_buffer( std::vector<VkCommandBuffer> &commandBuffers, VkCommandPool commandPool,
                     VkDevice device, int maxFramesInFlight ) {
  commandBuffers.resize( maxFramesInFlight );
  VkCommandBufferAllocateInfo allocInfo =
      Initializer::command_buffer_allocate_info( commandPool, (uint32_t)commandBuffers.size() );

  if ( vkAllocateCommandBuffers( device, &allocInfo, commandBuffers.data() ) != VK_SUCCESS ) {
    Logger::log( "Failed to allocate command buffers!", Logger::CRITICAL );
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

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>( bindings.size() );
  layoutInfo.pBindings = bindings.data();

  if ( vkCreateDescriptorSetLayout( vulkanDevice->device, &layoutInfo, nullptr,
                                    &descriptorSetLayout ) != VK_SUCCESS ) {
    Logger::log( "Failed to create descriptor set layout!", Logger::CRITICAL );
  }
}

void descriptor_pool( VulkanDevice *vulkanDevice, VkDescriptorPool &descriptorPool,
                      int maxFramesInFlight ) {
  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>( maxFramesInFlight );
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>( maxFramesInFlight );

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>( poolSizes.size() );
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>( maxFramesInFlight );

  if ( vkCreateDescriptorPool( vulkanDevice->device, &poolInfo, nullptr, &descriptorPool ) !=
       VK_SUCCESS ) {
    Logger::log( "Failed to create descriptor pool!", Logger::CRITICAL );
  }
}

void descriptor_sets( VulkanDevice *vulkanDevice, VkDescriptorSetLayout &descriptorSetLayout,
                      VkDescriptorPool &descriptorPool,
                      std::vector<VkDescriptorSet> &descriptorSets,
                      std::vector<VkBuffer> &uniformBuffers, VulkanImage *textureImage,
                      int maxFramesInFlight ) {
  std::vector<VkDescriptorSetLayout> layouts( maxFramesInFlight, descriptorSetLayout );
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>( maxFramesInFlight );
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets.resize( maxFramesInFlight );
  if ( vkAllocateDescriptorSets( vulkanDevice->device, &allocInfo, descriptorSets.data() ) !=
       VK_SUCCESS ) {
    Logger::log( "Failed to allocate descriptor sets!", Logger::CRITICAL );
  }

  for ( size_t i = 0; i < maxFramesInFlight; i++ ) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof( UniformBufferObject );

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImage->imageView;
    imageInfo.sampler = textureImage->sampler;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets( vulkanDevice->device, static_cast<uint32_t>( descriptorWrites.size() ),
                            descriptorWrites.data(), 0, nullptr );
  }
}

#pragma endregion Descriptor

}  // namespace Construct
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy