/**
 * @file vulkan_window.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief vulkan_window cpp file.
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "logger_helper.hpp"
#define GLFW_INCLUDE_VULKAN

#include <vulkan/vulkan_core.h>

#include <cstdint>  // Necessary for uint32_t
#include <cstring>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <vector>

#include "logger.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan_buffers.hpp"
#include "vulkan_command.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_initializers.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_window.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

#pragma region Core

VulkanWindow::VulkanWindow( std::string title ) : Window( title ) {
  init_vulkan();
}

void VulkanWindow::init_vulkan() {
  // Create our instance
  create_instance();

  // Setup debug messenger
  Debug::setup_debug_messenger( instance_, &debugMessenger_ );

  // Create surface
  create_surface();

  // Find & create vulkan device
  vulkanDevice_ = new VulkanDevice( instance_, surface_ );

  // Create swap chain / image views / render pass
  swapChain_ = new VulkanSwapChain( vulkanDevice_, window_, surface_ );

  // Create descriptor layouts
  create_descriptor_set_layout();

  // Create graphics pipeline
  pipeline_ = create_graphics_pipeline( swapChain_, vulkanDevice_->device,
                                        descriptorSetLayout_ );

  // Create frame buffers
  Buffer::create_framebuffers( swapChain_, vulkanDevice_->device );

  // Create command pool & buffer
  create_command_pool( vulkanDevice_, commandPool_ );

  // Create vertex buffer
  // Generate sierpinski triangle (broken with index buffer)
  // vertices_ = Shapes::generate_sierpinski_triangle( 6, vertices_ );

  Buffer::create_vertex_buffer( vertices_, vulkanDevice_, vertexBuffer_,
                                vertexBufferMemory_, commandPool_ );

  // Create Index Buffer
  Buffer::create_index_buffer( indices_, vulkanDevice_, indexBuffer_,
                               indexBufferMemory_, commandPool_ );

  create_uniform_buffers();
  create_descriptor_pool();
  create_descriptor_sets();

  // Create command buffer
  create_command_buffer( commandBuffers_, commandPool_, vulkanDevice_->device,
                         MAX_FRAMES_IN_FLIGHT );

  // Create render
  render_ = new VulkanRender( MAX_FRAMES_IN_FLIGHT, vulkanDevice_, swapChain_,
                              &commandBuffers_, &pipeline_ );
}

void VulkanWindow::deconstruct_window() {
  Logger::log( "Destroying vulkan..." );

  swapChain_->clear_swap_chain();

  destroy_graphics_pipeline( vulkanDevice_->device, pipeline_ );

  vkDestroyRenderPass( vulkanDevice_->device, swapChain_->renderPass, nullptr );

  render_->destroy();

  for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
    vkDestroyBuffer( vulkanDevice_->device, uniformBuffers_[i], nullptr );
    vkFreeMemory( vulkanDevice_->device, uniformBuffersMemory_[i], nullptr );
  }

  vkDestroyDescriptorPool( vulkanDevice_->device, descriptorPool_, nullptr );
  vkDestroyDescriptorSetLayout( vulkanDevice_->device, descriptorSetLayout_,
                                nullptr );

  vkDestroyBuffer( vulkanDevice_->device, indexBuffer_, nullptr );
  vkFreeMemory( vulkanDevice_->device, indexBufferMemory_, nullptr );

  vkDestroyBuffer( vulkanDevice_->device, vertexBuffer_, nullptr );
  vkFreeMemory( vulkanDevice_->device, vertexBufferMemory_, nullptr );

  vkDestroyCommandPool( vulkanDevice_->device, commandPool_, nullptr );

  vkDestroyDevice( vulkanDevice_->device, nullptr );

  if ( enableValidationLayers ) {
    Debug::destroy_debug_utils_messenger_ext( instance_, &debugMessenger_,
                                              nullptr );
  }

  vkDestroySurfaceKHR( instance_, surface_, nullptr );
  vkDestroyInstance( instance_, nullptr );

  Window::deconstruct_window();
}

void VulkanWindow::loop() {
  Window::loop();
  render_->draw_frame( vertexBuffer_, static_cast<uint32_t>( vertices_.size() ),
                       indexBuffer_, static_cast<uint32_t>( indices_.size() ),
                       uniformBuffersMapped_, descriptorSets_ );

  vkDeviceWaitIdle( vulkanDevice_->device );
}

void VulkanWindow::create_instance() {
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

  if ( vkCreateInstance( &createInfo, nullptr, &instance_ ) != VK_SUCCESS ) {
    Logger::log( "Failed to create instance!", Logger::CRITICAL );
  }
}

void VulkanWindow::create_surface() {
  if ( glfwCreateWindowSurface( instance_, window_, nullptr, &surface_ ) !=
       VK_SUCCESS ) {
    Logger::log( "Failed to create window surface!", Logger::CRITICAL );
  }
}

#pragma endregion Core

void VulkanWindow::create_descriptor_set_layout() {
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

  if ( vkCreateDescriptorSetLayout( vulkanDevice_->device, &layoutInfo, nullptr,
                                    &descriptorSetLayout_ ) != VK_SUCCESS ) {
    Logger::log( "Failed to create descriptor set layout!", Logger::CRITICAL );
  }
}

void VulkanWindow::create_uniform_buffers() {
  VkDeviceSize bufferSize = sizeof( UniformBufferObject );

  uniformBuffers_.resize( MAX_FRAMES_IN_FLIGHT );
  uniformBuffersMemory_.resize( MAX_FRAMES_IN_FLIGHT );
  uniformBuffersMapped_.resize( MAX_FRAMES_IN_FLIGHT );

  for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
    Buffer::create_buffer( bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           uniformBuffers_[i], uniformBuffersMemory_[i],
                           vulkanDevice_ );

    vkMapMemory( vulkanDevice_->device, uniformBuffersMemory_[i], 0, bufferSize,
                 0, &uniformBuffersMapped_[i] );
  }
}

void VulkanWindow::create_descriptor_pool() {
  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = static_cast<uint32_t>( MAX_FRAMES_IN_FLIGHT );

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;

  poolInfo.maxSets = static_cast<uint32_t>( MAX_FRAMES_IN_FLIGHT );

  if ( vkCreateDescriptorPool( vulkanDevice_->device, &poolInfo, nullptr,
                               &descriptorPool_ ) != VK_SUCCESS ) {
    Logger::log( "Failed to create descriptor pool!", Logger::CRITICAL );
  }
}

void VulkanWindow::create_descriptor_sets() {
  std::vector<VkDescriptorSetLayout> layouts( MAX_FRAMES_IN_FLIGHT,
                                              descriptorSetLayout_ );
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool_;
  allocInfo.descriptorSetCount = static_cast<uint32_t>( MAX_FRAMES_IN_FLIGHT );
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets_.resize( MAX_FRAMES_IN_FLIGHT );
  if ( vkAllocateDescriptorSets( vulkanDevice_->device, &allocInfo,
                                 descriptorSets_.data() ) != VK_SUCCESS ) {
    Logger::log( "Failed to allocate descriptor sets!", Logger::CRITICAL );
  }

  for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffers_[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof( UniformBufferObject );

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSets_[i];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;        // Optional
    descriptorWrite.pTexelBufferView = nullptr;  // Optional

    vkUpdateDescriptorSets( vulkanDevice_->device, 1, &descriptorWrite, 0,
                            nullptr );
  }
}

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
