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

#define GLFW_INCLUDE_VULKAN

#include "vulkan_window.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>  // Necessary for uint32_t
#include <cstring>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <vector>

#include "logger.hpp"
#include "vulkan_buffers.hpp"
#include "vulkan_construct.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_image.hpp"
#include "vulkan_render.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

#pragma region Core

VulkanWindow::VulkanWindow( std::string title ) : Window( title ) { init_vulkan(); }

void VulkanWindow::init_vulkan() {
  // Create our instance
  Construct::instance( instance_ );

  // Setup debug messenger
  Debug::setup_debug_messenger( instance_, &debugMessenger_ );

  // Create surface
  create_surface();

  // Find & create vulkan device
  vulkanDevice_ = new VulkanDevice( instance_, surface_ );

  // Create swap chain / image views / render pass
  swapChain_ = new VulkanSwapChain( vulkanDevice_, window_, surface_ );

  // Create descriptor layouts
  Construct::descriptor_set_layout( vulkanDevice_, descriptorSetLayout_ );

  // Create graphics pipeline
  pipeline_ = create_graphics_pipeline( swapChain_, vulkanDevice_, descriptorSetLayout_ );

  // Multisampling
  msaaColorBuffer_ = new VulkanImage();
  Image::create_color_resources( msaaColorBuffer_, vulkanDevice_, swapChain_ );

  // Depth buffer
  depthBuffer_ = new VulkanImage();
  Image::create_depth_resources( depthBuffer_, vulkanDevice_, swapChain_->extent );

  // Create frame buffers
  Buffer::create_framebuffers( swapChain_, depthBuffer_->imageView, msaaColorBuffer_->imageView,
                               vulkanDevice_->device );

  // Create command pool & buffer
  Construct::command_pool( vulkanDevice_, commandPool_ );

  // Create texture image / view / sampler
  textureImage_ = new VulkanTextureImage();
  Image::create_texture_image( vulkanDevice_, textureImage_, commandPool_, TEXTURE_PATH );
  Image::create_texture_image_view( vulkanDevice_->device, textureImage_ );
  Image::create_texture_sampler( vulkanDevice_, textureImage_ );

  // Create vertex buffer
  // Generate sierpinski triangle (broken with index buffer,
  // but it would be really cool if you fixed that)
  // vertices_ = Shapes::generate_sierpinski_triangle( 6, vertices_ );

  // mesh_ = Shapes::generate_triangle();
  // mesh_ = Shapes::generate_square();
  mesh_ = load_mesh( MODEL_PATH );

  // mesh_ = Shapes::generate_sierpinski_triangle( mesh_, 2 );

  Buffer::create_vertex_buffer( mesh_->vertices, vulkanDevice_, vertexBuffer_, vertexBufferMemory_,
                                commandPool_ );

  // Create Index Buffer
  Buffer::create_index_buffer( mesh_->indices, vulkanDevice_, indexBuffer_, indexBufferMemory_,
                               commandPool_ );

  // Create uniform buffers
  Construct::uniform_buffers( vulkanDevice_, uniformBuffers_, uniformBuffersMemory_,
                              uniformBuffersMapped_, MAX_FRAMES_IN_FLIGHT );

  // Create descriptor pool
  Construct::descriptor_pool( vulkanDevice_, descriptorPool_, MAX_FRAMES_IN_FLIGHT );

  // Create descriptor sets
  Construct::descriptor_sets( vulkanDevice_, descriptorSetLayout_, descriptorPool_, descriptorSets_,
                              uniformBuffers_, textureImage_, MAX_FRAMES_IN_FLIGHT );

  // Create command buffer
  Construct::command_buffer( commandBuffers_, commandPool_, vulkanDevice_->device,
                             MAX_FRAMES_IN_FLIGHT );

  // Create render
  render_ = new VulkanRender( MAX_FRAMES_IN_FLIGHT, vulkanDevice_, swapChain_, &commandBuffers_,
                              pipeline_ );
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

  textureImage_->destroy( vulkanDevice_->device );
  depthBuffer_->destroy( vulkanDevice_->device );
  msaaColorBuffer_->destroy( vulkanDevice_->device );

  vkDestroyDescriptorSetLayout( vulkanDevice_->device, descriptorSetLayout_, nullptr );

  vkDestroyBuffer( vulkanDevice_->device, indexBuffer_, nullptr );
  vkFreeMemory( vulkanDevice_->device, indexBufferMemory_, nullptr );

  vkDestroyBuffer( vulkanDevice_->device, vertexBuffer_, nullptr );
  vkFreeMemory( vulkanDevice_->device, vertexBufferMemory_, nullptr );

  vkDestroyCommandPool( vulkanDevice_->device, commandPool_, nullptr );

  vkDestroyDevice( vulkanDevice_->device, nullptr );

  if ( enableValidationLayers ) {
    Debug::destroy_debug_utils_messenger_ext( instance_, &debugMessenger_, nullptr );
  }

  vkDestroySurfaceKHR( instance_, surface_, nullptr );
  vkDestroyInstance( instance_, nullptr );

  Window::deconstruct_window();
}

void VulkanWindow::loop() {
  Window::loop();
  render_->draw_frame( vertexBuffer_, static_cast<uint32_t>( mesh_->vertices.size() ), indexBuffer_,
                       static_cast<uint32_t>( mesh_->indices.size() ), uniformBuffersMapped_,
                       descriptorSets_, depthBuffer_, msaaColorBuffer_ );

  vkDeviceWaitIdle( vulkanDevice_->device );
}

void VulkanWindow::create_surface() {
  if ( glfwCreateWindowSurface( instance_, window_, nullptr, &surface_ ) != VK_SUCCESS ) {
    Logger::log( "Failed to create window surface!", Logger::CRITICAL );
  }
}

#pragma endregion Core

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
