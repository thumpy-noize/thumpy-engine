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

// #include <vulkan/vulkan_core.h>

#include <vulkan/vulkan_raii.hpp>

#include "logger.hpp"
#include "logger_helper.hpp"
// #include "vulkan_buffers.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_device.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_initializers.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Construct {

// void instance( VkInstance &instance ) {
//   if ( enableValidationLayers && !check_validation_layer_support() ) {
//     throw VulkanNotCompatible( "validation layers requested, but not available!" );
//   }

//   VkApplicationInfo appInfo = Initializer::application_info();

//   VkInstanceCreateInfo createInfo{};
//   createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//   createInfo.pApplicationInfo = &appInfo;

//   auto extensions = get_required_extensions();
//   createInfo.enabledExtensionCount = static_cast<uint32_t>( extensions.size() );
//   createInfo.ppEnabledExtensionNames = extensions.data();

//   VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
//   if ( enableValidationLayers ) {
//     createInfo.enabledLayerCount = static_cast<uint32_t>( validationLayers.size() );
//     createInfo.ppEnabledLayerNames = validationLayers.data();

//     Debug::populate_debug_messenger_create_info( debugCreateInfo );
//     createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
//   } else {
//     createInfo.enabledLayerCount = 0;

//     createInfo.pNext = nullptr;
//   }

//   if ( vkCreateInstance( &createInfo, nullptr, &instance ) != VK_SUCCESS ) {
//     throw VulkanNotCompatible( "Failed to create instance!" );
//   }
// }

void raii_instance( vk::raii::Instance &instance, vk::raii::Context &context ) {
  Logger::log( "Constructing RAII Vulkan instance...", Logger::DEBUG );

  // Get the required layers
  std::vector<char const *> requiredLayers;
  if ( enableValidationLayers ) {
    requiredLayers.assign( validationLayers.begin(), validationLayers.end() );
  }

  // Check if the required layers are supported by the Vulkan implementation.
  // Get required layers
  auto layerProperties = context.enumerateInstanceLayerProperties();
  auto unsupportedLayerIt =
      std::ranges::find_if( requiredLayers, [&layerProperties]( auto const &requiredLayer ) {
        return std::ranges::none_of( layerProperties, [requiredLayer]( auto const &layerProperty ) {
          return strcmp( layerProperty.layerName, requiredLayer ) == 0;
        } );
      } );

  // Validate layers are supported
  if ( unsupportedLayerIt != requiredLayers.end() ) {
    throw std::runtime_error( "Required layer not supported: " +
                              std::string( *unsupportedLayerIt ) );
  }

  // Get the required extensions.
  std::vector<const char *> requiredExtensions = getRequiredInstanceExtensions();

  // Check if the required extensions are supported by the Vulkan implementation.
  auto extensionProperties = context.enumerateInstanceExtensionProperties();
  auto unsupportedPropertyIt = std::ranges::find_if(
      requiredExtensions, [&extensionProperties]( auto const &requiredExtension ) {
        return std::ranges::none_of(
            extensionProperties, [requiredExtension]( auto const &extensionProperty ) {
              return strcmp( extensionProperty.extensionName, requiredExtension ) == 0;
            } );
      } );

  if ( unsupportedPropertyIt != requiredExtensions.end() ) {
    throw std::runtime_error( "Required extension not supported: " +
                              std::string( *unsupportedPropertyIt ) );
  }

  // Create app info
  Logger::log( "Constructing app info...", Logger::DEBUG );
  constexpr vk::ApplicationInfo appInfo{
      .pApplicationName = "Thumpy Engine Editor",  // TODO: Convert to variable
      .applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
      .pEngineName = "Thumpy Engine",  // TODO: Convert to variable
      .engineVersion = VK_MAKE_VERSION( 1, 0, 0 ),
      .apiVersion = vk::ApiVersion14 };

  // Create instance info
  Logger::log( "Constructing instance info...", Logger::DEBUG );
  vk::InstanceCreateInfo createInfo{
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>( requiredLayers.size() ),
      .ppEnabledLayerNames = requiredLayers.data(),
      .enabledExtensionCount = static_cast<uint32_t>( requiredExtensions.size() ),
      .ppEnabledExtensionNames = requiredExtensions.data() };

  // Create instance
  Logger::log( "Constructing instance...", Logger::DEBUG );
  instance = vk::raii::Instance( context, createInfo );

  Logger::log( "Constructed RAII Vulkan instance.", Logger::DEBUG );
}

std::vector<const char *> getRequiredInstanceExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

  std::vector extensions( glfwExtensions, glfwExtensions + glfwExtensionCount );
  if ( enableValidationLayers ) {
    extensions.push_back( vk::EXTDebugUtilsExtensionName );
  }

  Logger::log( "Extensions:", Logger::DEBUG );

  for ( int i = 0; i < extensions.size(); i++ ) {
    Logger::log( extensions[i], Logger::DEBUG );
  }

  return extensions;
}

// void uniform_buffers( VulkanDevice *vulkanDevice, UniformBuffers *uniformBuffers,
//                       int maxFramesInFlight ) {
//   // VkDeviceSize bufferSize = sizeof( UniformBufferObject );

//   // uniformBuffers->buffers.resize( maxFramesInFlight );
//   // uniformBuffers->memory.resize( maxFramesInFlight );
//   // uniformBuffers->mapped.resize( maxFramesInFlight );

//   // for ( size_t i = 0; i < maxFramesInFlight; i++ ) {
//   //   Buffer::create_buffer(
//   //       bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//   //       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//   //       uniformBuffers->buffers[i], uniformBuffers->memory[i], vulkanDevice );

//   //   vkMapMemory( vulkanDevice->device, uniformBuffers->memory[i], 0, bufferSize, 0,
//   //                &uniformBuffers->mapped[i] );
//   // }
// }

void command_pool( vk::raii::CommandPool &commandPool,
                   std::shared_ptr<VulkanDevice> vulkanDevice ) {
  Logger::log( "Creating command pool...", Logger::DEBUG );

  // Create pool info
  vk::CommandPoolCreateInfo poolInfo{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                      .queueFamilyIndex = vulkanDevice->queueIndex };
  // Create pool
  commandPool = vk::raii::CommandPool( vulkanDevice->device, poolInfo );

  // QueueFamilyIndices queueFamilyIndices =
  //     vulkanDevice->find_queue_families( vulkanDevice->physicalDevice );

  // VkCommandPoolCreateInfo poolInfo =
  //     Initializer::pool_info( queueFamilyIndices.graphicsFamily.value() );

  // if ( vkCreateCommandPool( vulkanDevice->device, &poolInfo, nullptr, &commandPool ) !=
  //      VK_SUCCESS ) {
  //   Logger::log( "Failed to create command pool!", Logger::CRITICAL );
  // }
}

void command_buffer( std::shared_ptr<Construct::CommandPool> commandPool,
                     std::shared_ptr<VulkanDevice> vulkanDevice, uint32_t maxFramesInFlight ) {
  Logger::log( "Creating command buffer...", Logger::DEBUG );

  // Create allocation info
  vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool->pool,
                                           .level = vk::CommandBufferLevel::ePrimary,
                                           .commandBufferCount = maxFramesInFlight };
  // Create command buffer
  commandPool->buffers = vk::raii::CommandBuffers( vulkanDevice->device, allocInfo );

  // commandBuffers.resize( maxFramesInFlight );
  // VkCommandBufferAllocateInfo allocInfo =
  //     Initializer::command_buffer_allocate_info( commandPool, (uint32_t)commandBuffers.size()
  // );

  // if ( vkAllocateCommandBuffers( device, &allocInfo, commandBuffers.data() ) != VK_SUCCESS ) {
  //   Logger::log( "Failed to allocate command buffers!", Logger::CRITICAL );
  // }
}

#pragma region Descriptor

void descriptor_set_layout( std::shared_ptr<VulkanDevice> vulkanDevice,
                            vk::raii::DescriptorSetLayout &descriptorSetLayout ) {
  Logger::log( "Creating descriptor set layout...", Logger::DEBUG );

  // Bind uniform buffer layout
  // vk::DescriptorSetLayoutBinding uboLayoutBinding( 0, vk::DescriptorType::eUniformBuffer, 1,
  //                                                  vk::ShaderStageFlagBits::eVertex, nullptr );

  // Create bindings array
  std::array bindings = {
      vk::DescriptorSetLayoutBinding( 0, vk::DescriptorType::eUniformBuffer, 1,
                                      vk::ShaderStageFlagBits::eVertex, nullptr ),
      vk::DescriptorSetLayoutBinding( 1, vk::DescriptorType::eCombinedImageSampler, 1,
                                      vk::ShaderStageFlagBits::eFragment, nullptr ) };

  // Create layout info
  // vk::DescriptorSetLayoutCreateInfo layoutInfo{ .bindingCount = 1, .pBindings = &uboLayoutBinding
  // };
  vk::DescriptorSetLayoutCreateInfo layoutInfo{
      .bindingCount = static_cast<uint32_t>( bindings.size() ), .pBindings = bindings.data() };

  // Set descriptor set layout
  descriptorSetLayout = vk::raii::DescriptorSetLayout( vulkanDevice->device, layoutInfo );

  //   // VkDescriptorSetLayoutBinding uboLayoutBinding{};
  //   // uboLayoutBinding.binding = 0;
  //   // uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  //   // uboLayoutBinding.descriptorCount = 1;

  //   // uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  //   // uboLayoutBinding.pImmutableSamplers = nullptr;  // Optional

  //   // VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  //   // samplerLayoutBinding.binding = 1;
  //   // samplerLayoutBinding.descriptorCount = 1;
  //   // samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  //   // samplerLayoutBinding.pImmutableSamplers = nullptr;
  //   // samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  //   // std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding,
  //   samplerLayoutBinding
  //   // }; VkDescriptorSetLayoutCreateInfo layoutInfo{}; layoutInfo.sType =
  //   // VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO; layoutInfo.bindingCount =
  //   // static_cast<uint32_t>( bindings.size() ); layoutInfo.pBindings = bindings.data();

  //   // if ( vkCreateDescriptorSetLayout( vulkanDevice->device, &layoutInfo, nullptr,
  //   //                                   &descriptorSetLayout ) != VK_SUCCESS ) {
  //   //   Logger::log( "Failed to create descriptor set layout!", Logger::CRITICAL );
  //   // }
}

void descriptor_pool( std::shared_ptr<VulkanDevice> vulkanDevice,
                      vk::raii::DescriptorPool &descriptorPool, uint32_t maxFramesInFlight ) {
  Logger::log( "Creating descriptor pool...", Logger::DEBUG );

  // Get pool size
  // vk::DescriptorPoolSize poolSize( vk::DescriptorType::eUniformBuffer, maxFramesInFlight );
  std::array poolSize{
      vk::DescriptorPoolSize( vk::DescriptorType::eUniformBuffer, maxFramesInFlight ),
      vk::DescriptorPoolSize( vk::DescriptorType::eCombinedImageSampler, maxFramesInFlight ) };

  // Create pool info
  vk::DescriptorPoolCreateInfo poolInfo{
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = maxFramesInFlight,
      .poolSizeCount = static_cast<uint32_t>( poolSize.size() ),
      .pPoolSizes = poolSize.data() };

  // Create descriptor pool
  descriptorPool = vk::raii::DescriptorPool( vulkanDevice->device, poolInfo );
  //   // std::array<VkDescriptorPoolSize, 2> poolSizes{};
  //   // poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  //   // poolSizes[0].descriptorCount = static_cast<uint32_t>( maxFramesInFlight );
  //   // poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  //   // poolSizes[1].descriptorCount = static_cast<uint32_t>( maxFramesInFlight );

  //   // VkDescriptorPoolCreateInfo poolInfo{};
  //   // poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  //   // poolInfo.poolSizeCount = static_cast<uint32_t>( poolSizes.size() );
  //   // poolInfo.pPoolSizes = poolSizes.data();
  //   // poolInfo.maxSets = static_cast<uint32_t>( maxFramesInFlight );

  //   // if ( vkCreateDescriptorPool( vulkanDevice->device, &poolInfo, nullptr, &descriptorPool )
  //   !=
  //   //      VK_SUCCESS ) {
  //   //   Logger::log( "Failed to create descriptor pool!", Logger::CRITICAL );
  //   // }
}

void descriptor_sets( std::shared_ptr<VulkanDevice> vulkanDevice,
                      std::shared_ptr<Descriptors> descriptors,
                      std::vector<vk::raii::Buffer> &uniformBuffers,
                      std::shared_ptr<Image::VulkanTextureImage> vulkanTextureImage,
                      uint32_t maxFramesInFlight ) {
  Logger::log( "Creating descriptor sets...", Logger::DEBUG );

  // Create descriptor set layout
  std::vector<vk::DescriptorSetLayout> layouts( maxFramesInFlight, *descriptors->setLayout );

  // Create allocation info
  vk::DescriptorSetAllocateInfo allocInfo{
      .descriptorPool = descriptors->pool,
      .descriptorSetCount = static_cast<uint32_t>( layouts.size() ),
      .pSetLayouts = layouts.data() };

  // Clear sets (not sure if this is needed)
  descriptors->sets.clear();

  // Allocate descriptor sets
  descriptors->sets = vulkanDevice->device.allocateDescriptorSets( allocInfo );

  for ( size_t i = 0; i < maxFramesInFlight; i++ ) {
    // Create descriptor buffer info
    vk::DescriptorBufferInfo bufferInfo{
        .buffer = uniformBuffers[i], .offset = 0, .range = sizeof( UniformBufferObject ) };

    // Create image info
    vk::DescriptorImageInfo imageInfo{ .sampler = vulkanTextureImage->textureSampler,
                                       .imageView = vulkanTextureImage->imageView,
                                       .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal };

    // Create descriptor set
    // vk::WriteDescriptorSet descriptorWrite{ .dstSet = descriptors->sets[i],
    //                                         .dstBinding = 0,
    //                                         .dstArrayElement = 0,
    //                                         .descriptorCount = 1,
    //                                         .descriptorType = vk::DescriptorType::eUniformBuffer,
    //                                         .pBufferInfo = &bufferInfo };

    std::array descriptorWrites{
        vk::WriteDescriptorSet{ .dstSet = descriptors->sets[i],
                                .dstBinding = 0,
                                .dstArrayElement = 0,
                                .descriptorCount = 1,
                                .descriptorType = vk::DescriptorType::eUniformBuffer,
                                .pBufferInfo = &bufferInfo },
        vk::WriteDescriptorSet{ .dstSet = descriptors->sets[i],
                                .dstBinding = 1,
                                .dstArrayElement = 0,
                                .descriptorCount = 1,
                                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                                .pImageInfo = &imageInfo } };
    // Update descriptor sets
    vulkanDevice->device.updateDescriptorSets( descriptorWrites, {} );
  }

  //   // std::vector<VkDescriptorSetLayout> layouts( maxFramesInFlight, descriptors->setLayout );
  //   // VkDescriptorSetAllocateInfo allocInfo{};
  //   // allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  //   // allocInfo.descriptorPool = descriptors->pool;
  //   // allocInfo.descriptorSetCount = static_cast<uint32_t>( maxFramesInFlight );
  //   // allocInfo.pSetLayouts = layouts.data();

  //   // descriptors->sets.resize( maxFramesInFlight );
  //   // if ( vkAllocateDescriptorSets( vulkanDevice->device, &allocInfo, descriptors->sets.data()
  //   )
  //   !=
  //   //      VK_SUCCESS ) {
  //   //   Logger::log( "Failed to allocate descriptor sets!", Logger::CRITICAL );
  //   // }

  //   // for ( size_t i = 0; i < maxFramesInFlight; i++ ) {
  //   //   VkDescriptorBufferInfo bufferInfo{};
  //   //   bufferInfo.buffer = uniformBuffers[i];
  //   //   bufferInfo.offset = 0;
  //   //   bufferInfo.range = sizeof( UniformBufferObject );

  //   //   VkDescriptorImageInfo imageInfo{};
  //   //   imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  //   //   imageInfo.imageView = textureImage->imageView;
  //   //   imageInfo.sampler = textureImage->sampler;

  //   //   std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

  //   //   descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  //   //   descriptorWrites[0].dstSet = descriptors->sets[i];
  //   //   descriptorWrites[0].dstBinding = 0;
  //   //   descriptorWrites[0].dstArrayElement = 0;
  //   //   descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  //   //   descriptorWrites[0].descriptorCount = 1;
  //   //   descriptorWrites[0].pBufferInfo = &bufferInfo;

  //   //   descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  //   //   descriptorWrites[1].dstSet = descriptors->sets[i];
  //   //   descriptorWrites[1].dstBinding = 1;
  //   //   descriptorWrites[1].dstArrayElement = 0;
  //   //   descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  //   //   descriptorWrites[1].descriptorCount = 1;
  //   //   descriptorWrites[1].pImageInfo = &imageInfo;

  //   //   vkUpdateDescriptorSets( vulkanDevice->device, static_cast<uint32_t>(
  //   descriptorWrites.size()
  //   //   ),
  //   //                           descriptorWrites.data(), 0, nullptr );
  //   // }
}

#pragma endregion Descriptor

}  // namespace Construct
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy