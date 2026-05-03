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
    throw VulkanNotCompatible(
        ( "Required layer not supported: " + std::string( *unsupportedLayerIt ) ).c_str() );
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
    throw VulkanNotCompatible(
        ( "Required extension not supported: " + std::string( *unsupportedPropertyIt ) ).c_str() );
  }

  // Create application info
  constexpr vk::ApplicationInfo appInfo = Initializer::application_info();

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

void command_pool( vk::raii::CommandPool &commandPool,
                   std::shared_ptr<VulkanDevice> vulkanDevice ) {
  Logger::log( "Creating command pool...", Logger::DEBUG );

  // Create pool info
  vk::CommandPoolCreateInfo poolInfo = Initializer::pool_info( vulkanDevice->queueIndex );

  // Create pool
  commandPool = vk::raii::CommandPool( vulkanDevice->device, poolInfo );
}

void command_buffer( std::shared_ptr<Construct::CommandPool> commandPool,
                     std::shared_ptr<VulkanDevice> vulkanDevice, uint32_t maxFramesInFlight ) {
  Logger::log( "Creating command buffer...", Logger::DEBUG );

  // Create allocation info
  vk::CommandBufferAllocateInfo allocInfo =
      Initializer::command_buffer_allocate_info( commandPool->pool, maxFramesInFlight );

  // Create command buffer
  commandPool->buffers = vk::raii::CommandBuffers( vulkanDevice->device, allocInfo );
}

#pragma region Descriptor

void descriptor_set_layout( std::shared_ptr<VulkanDevice> vulkanDevice,
                            vk::raii::DescriptorSetLayout &descriptorSetLayout ) {
  Logger::log( "Creating descriptor set layout...", Logger::DEBUG );

  // Create bindings array
  std::array bindings = {
      vk::DescriptorSetLayoutBinding( 0, vk::DescriptorType::eUniformBuffer, 1,
                                      vk::ShaderStageFlagBits::eVertex, nullptr ),
      vk::DescriptorSetLayoutBinding( 1, vk::DescriptorType::eCombinedImageSampler, 1,
                                      vk::ShaderStageFlagBits::eFragment, nullptr ) };

  // Create layout info
  vk::DescriptorSetLayoutCreateInfo layoutInfo{
      .bindingCount = static_cast<uint32_t>( bindings.size() ), .pBindings = bindings.data() };

  // Set descriptor set layout
  descriptorSetLayout = vk::raii::DescriptorSetLayout( vulkanDevice->device, layoutInfo );
}

void descriptor_pool( std::shared_ptr<VulkanDevice> vulkanDevice,
                      vk::raii::DescriptorPool &descriptorPool, uint32_t maxFramesInFlight ) {
  Logger::log( "Creating descriptor pool...", Logger::DEBUG );

  // Get pool size
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
}

#pragma endregion Descriptor

}  // namespace Construct
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy