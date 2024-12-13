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

#include "vulkan_render.hpp"

#define GLFW_INCLUDE_VULKAN

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
#include "vulkan_helper.hpp"
#include "vulkan_image.hpp"
#include "vulkan_window.hpp"

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
  descriptors_ = new Descriptors();
  Construct::descriptor_set_layout( vulkanDevice_, descriptors_->setLayout );

  // Create graphics pipeline
  pipeline_ = create_graphics_pipeline( swapChain_, vulkanDevice_, descriptors_->setLayout );

  // Multisampling
  msaaColorBuffer_ = new VulkanImage();
  Image::create_color_resources( msaaColorBuffer_, vulkanDevice_, swapChain_ );

  // Depth buffer
  depthBuffer_ = new VulkanImage();
  Image::create_depth_resources( depthBuffer_, vulkanDevice_, swapChain_->extent );

  // Create frame buffers
  Buffer::create_framebuffers( swapChain_, depthBuffer_->imageView, msaaColorBuffer_->imageView,
                               vulkanDevice_->device );

  // Create command pool
  commandPool_ = new Construct::CommandPool();
  Construct::command_pool( vulkanDevice_, commandPool_->pool );

  // Create texture image / view / sampler
  textureImage_ = new VulkanTextureImage();
  Image::create_texture_image( vulkanDevice_, textureImage_, commandPool_->pool, TEXTURE_PATH );
  Image::create_texture_image_view( vulkanDevice_->device, textureImage_ );
  Image::create_texture_sampler( vulkanDevice_, textureImage_ );

  // Create vertex buffer

  // mesh_ = Shapes::generate_triangle();
  // mesh_ = Shapes::generate_square();
  mesh_ = load_mesh( MODEL_PATH );

  // mesh_ = Shapes::generate_sierpinski_triangle( mesh_, 1 );

  vertexBuffer_ = new Buffer::Buffer();
  Buffer::create_vertex_buffer( mesh_->vertices, vulkanDevice_, vertexBuffer_, commandPool_->pool );

  // Create Index Buffer
  indexBuffer_ = new Buffer::Buffer();
  Buffer::create_index_buffer( mesh_->indices, vulkanDevice_, indexBuffer_, commandPool_->pool );

  // Create uniform buffers
  uniformBuffers_ = new Construct::UniformBuffers();
  Construct::uniform_buffers( vulkanDevice_, uniformBuffers_, MAX_FRAMES_IN_FLIGHT );

  // Create descriptor pool
  Construct::descriptor_pool( vulkanDevice_, descriptors_->pool, MAX_FRAMES_IN_FLIGHT );

  // Create descriptor sets
  Construct::descriptor_sets( vulkanDevice_, descriptors_, uniformBuffers_->buffers, textureImage_,
                              MAX_FRAMES_IN_FLIGHT );

  // Create command buffer
  Construct::command_buffer( commandPool_->buffers, commandPool_->pool, vulkanDevice_->device,
                             MAX_FRAMES_IN_FLIGHT );

  // Create render
  render_ = new VulkanRender( MAX_FRAMES_IN_FLIGHT, vulkanDevice_, swapChain_,
                              &commandPool_->buffers, pipeline_ );
}

void VulkanWindow::deconstruct_window() {
  Logger::log( "Destroying vulkan..." );

  swapChain_->clear_swap_chain();

  destroy_graphics_pipeline( vulkanDevice_->device, pipeline_ );

  vkDestroyRenderPass( vulkanDevice_->device, swapChain_->renderPass, nullptr );

  render_->destroy();

  for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
    vkDestroyBuffer( vulkanDevice_->device, uniformBuffers_->buffers[i], nullptr );
    vkFreeMemory( vulkanDevice_->device, uniformBuffers_->memory[i], nullptr );
  }

  vkDestroyDescriptorPool( vulkanDevice_->device, descriptors_->pool, nullptr );

  textureImage_->destroy( vulkanDevice_->device );
  depthBuffer_->destroy( vulkanDevice_->device );
  msaaColorBuffer_->destroy( vulkanDevice_->device );

  vkDestroyDescriptorSetLayout( vulkanDevice_->device, descriptors_->setLayout, nullptr );

  indexBuffer_->destroy( vulkanDevice_->device );

  vertexBuffer_->destroy( vulkanDevice_->device );

  // vkDestroyCommandPool( vulkanDevice_->device, commandPool_, nullptr );
  commandPool_->destroy( vulkanDevice_->device );

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
  render_->draw_frame( vertexBuffer_->buffer, static_cast<uint32_t>( mesh_->vertices.size() ),
                       indexBuffer_->buffer, static_cast<uint32_t>( mesh_->indices.size() ),
                       uniformBuffers_->mapped, descriptors_->sets, depthBuffer_,
                       msaaColorBuffer_ );

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
