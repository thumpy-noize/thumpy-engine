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

// #include <vulkan/vulkan_core.h>
#include "vulkan_window.hpp"

#include <cstddef>
#include <cstdint>  // Necessary for uint32_t
#include <cstring>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <vector>

#include "logger.hpp"
#include "vulkan_debug.hpp"

// #include "vulkan_buffers.hpp"
// #include "vulkan_construct.hpp"
// #include "vulkan_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

#pragma region Core

VulkanWindow::VulkanWindow( std::string title ) : Window( title ) {}  // init_vulkan(); }

void VulkanWindow::init_vulkan() {
  // if ( glfwVulkanSupported() == GLFW_FALSE ) {
  //   Logger::log( "GLFW does not have Vulkan support!", Logger::ERROR_LOG );
  //   throw VulkanNotCompatible( "GLFW does not have Vulkan support!" );
  //   return;
  // }

  // Logger::log( "Constructing Vulkan window...", Logger::DEBUG );

  // // Construct instance
  // Construct::instance( instance_, context_ );

  // // Setup debug messenger
  // Debug::setup_debug_messenger( instance_, debugMessenger_ );

  // // Create surface
  // create_surface();

  init_surface();

  // Construct Vulkan device
  vulkanDevice_ = std::make_shared<VulkanDevice>( instance_, *surface_ );

  // Construct swap chain
  swapChain_ = std::make_shared<VulkanSwapChain>( vulkanDevice_, window_, surface_ );

  // Create descriptors
  // descriptors_ = std::make_shared<Descriptors>();
  // Construct::descriptor_set_layout( vulkanDevice_, descriptors_->setLayout );
  init_descriptors();

  // Set shader
  init_shader();

  // Construct pipeline
  pipeline_ =
      create_graphics_pipeline( vulkanDevice_, swapChain_, descriptors_->setLayout, SHADER_PATH );

  // Construct command pool
  commandPool_ = std::make_shared<Construct::CommandPool>();
  Construct::command_pool( commandPool_->pool, vulkanDevice_ );

  // Create msaa color image
  colorImage_ = std::make_shared<Image::VulkanImage>();
  Image::create_color_resources( colorImage_, vulkanDevice_, swapChain_ );

  // Create depth image
  depthBuffer_ = std::make_shared<Image::VulkanImage>();
  Image::create_depth_resources( depthBuffer_, vulkanDevice_, swapChain_->swapChainExtent );

  // Create texture image
  init_texture();

  // Create texture image view
  Image::create_texture_image_view( vulkanTextureImage_, vulkanDevice_ );

  // Create image sampler
  Image::create_texture_sampler( vulkanTextureImage_, vulkanDevice_ );

  // Load model
  init_mesh();

  // Create vertex buffer
  vertexBuffer_ = std::make_shared<Buffer::Buffer>();
  Buffer::create_vertex_buffer( mesh_->vertices, vulkanDevice_, vertexBuffer_, commandPool_->pool );

  // Create index buffer
  indexBuffer_ = std::make_shared<Buffer::Buffer>();
  Buffer::create_index_buffer( mesh_->indices, vulkanDevice_, indexBuffer_, commandPool_->pool );

  // Create uniform buffers
  uniformBuffers_ = std::make_shared<Buffer::UniformBuffers>();
  Buffer::create_uniform_buffers( uniformBuffers_, vulkanDevice_, MAX_FRAMES_IN_FLIGHT );

  // Create descriptor pool
  Construct::descriptor_pool( vulkanDevice_, descriptors_->pool, MAX_FRAMES_IN_FLIGHT );
  // Create descriptor sets
  Construct::descriptor_sets( vulkanDevice_, descriptors_, uniformBuffers_->buffers,
                              vulkanTextureImage_, MAX_FRAMES_IN_FLIGHT );

  // Create command buffer
  Construct::command_buffer( commandPool_, vulkanDevice_, MAX_FRAMES_IN_FLIGHT );

  // Construct render
  render_ = std::make_shared<VulkanRender>( vulkanDevice_, swapChain_, pipeline_, commandPool_,
                                            MAX_FRAMES_IN_FLIGHT );
}

void VulkanWindow::init_surface() {
  if ( glfwVulkanSupported() == GLFW_FALSE ) {
    Logger::log( "GLFW does not have Vulkan support!", Logger::ERROR_LOG );
    throw VulkanNotCompatible( "GLFW does not have Vulkan support!" );
    return;
  }

  Logger::log( "Constructing Vulkan window...", Logger::DEBUG );

  // Construct instance
  Construct::instance( instance_, context_ );

  // Setup debug messenger
  Debug::setup_debug_messenger( instance_, debugMessenger_ );

  // Create surface
  create_surface();
}

void VulkanWindow::init_descriptors() {
  descriptors_ = std::make_shared<Descriptors>();
  Construct::descriptor_set_layout( vulkanDevice_, descriptors_->setLayout );
}

void VulkanWindow::init_texture() {
  Logger::log( "Initialing texture...", Logger::DEBUG );

  // Setup texture
  vulkanTextureImage_ = std::make_shared<Image::VulkanTextureImage>();
  Image::create_texture_image( vulkanDevice_, commandPool_->pool, vulkanTextureImage_,
                               TEXTURE_PATH );
}

void VulkanWindow::init_mesh() {
  Logger::log( "Initialing mesh...", Logger::DEBUG );

  // Setup mesh
  mesh_ = load_mesh( MODEL_PATH );
}

void VulkanWindow::init_shader() { SHADER_PATH = "texture_shader.slang.spv"; }

void VulkanWindow::deconstruct_window() {
  Logger::log( "Destroying Vulkan window...", Logger::DEBUG );

  // Wait for vulkan device
  if ( vulkanDevice_ ) {
    vulkanDevice_->device.waitIdle();
  }

  // Clear swap chain
  if ( swapChain_ ) {
    swapChain_->clear_swap_chain();
  }
}

void VulkanWindow::loop() {
  Window::loop();

  // Validate device exist
  if ( vulkanDevice_ ) {
    // Draw frame
    render_->draw_frame( framebufferResized, vertexBuffer_, mesh_->vertices.size(), indexBuffer_,
                         mesh_->indices.size(), uniformBuffers_->mapped, depthBuffer_, colorImage_,
                         descriptors_ );
  }
}

void VulkanWindow::create_surface() {
  // Create surface
  VkSurfaceKHR _surface;
  if ( glfwCreateWindowSurface( *instance_, window_, nullptr, &_surface ) != 0 ) {
    // Validate surface creation
    Logger::log( "Failed to create window surface.", Logger::ERROR_LOG );
    throw VulkanRuntimeError( "Failed to create window surface!" );
  }

  // Set surface to instance
  surface_ = std::make_shared<vk::raii::SurfaceKHR>( instance_, _surface );
}

#pragma endregion Core

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
