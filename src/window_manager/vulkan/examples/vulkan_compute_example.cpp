/**
 * @file vulkan_triangle.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Creates a window with a sierpinski's triangle
 * @version 0.1
 * @date 2024-12-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "vulkan_compute_example.hpp"

#include "logger.hpp"
#include "vulkan/vulkan_buffers.hpp"
#include "vulkan/vulkan_construct.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan/vulkan_helper.hpp"
#include "vulkan/vulkan_image.hpp"
#include "vulkan/vulkan_render.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Examples {

VulkanComputeExample::VulkanComputeExample( std::string title ) : VulkanWindow( title ) {}

void VulkanComputeExample::init_vulkan() {
  // Create Vulkan instance and surface
  init_surface();

  // Construct Vulkan device
  vulkanDevice_ = std::make_shared<VulkanDevice>( instance_, *surface_ );

  // Construct swap chain
  swapChain_ = std::make_shared<VulkanSwapChain>( vulkanDevice_, window_, surface_ );

  // Create descriptors
  // TODO: Setup compute descriptors

  init_descriptors_layout();

  // Set shader
  init_shader();

  // Construct pipeline
  pipeline_ =
      create_graphics_pipeline( vulkanDevice_, swapChain_, descriptors_->setLayout, SHADER_PATH );

  // TODO: Create compute pipeline

  // Construct command pool
  commandPool_ = std::make_shared<Construct::CommandPool>();
  Construct::command_pool( commandPool_->pool, vulkanDevice_ );

  // TODO: Create shader storage buffers

  // TODO: Remove unnecessary buffers ( texture / vertex / index )

  // Init image resources
  init_image_resources();

  // Create texture image
  init_texture();

  // Create texture image view
  Image::create_texture_image_view( vulkanTextureImage_, vulkanDevice_ );

  // Create image sampler
  Image::create_texture_sampler( vulkanTextureImage_, vulkanDevice_ );

  // Load model
  init_mesh();

  // Init buffers
  init_buffers();

  // Init descriptors
  init_descriptors();

  // TODO: Create compute descriptor sets

  // Create command buffer
  Construct::command_buffer( commandPool_, vulkanDevice_, MAX_FRAMES_IN_FLIGHT );

  // TODO: Create compute command buffers

  // Construct render
  render_ = std::make_shared<VulkanRender>( vulkanDevice_, swapChain_, pipeline_, commandPool_,
                                            MAX_FRAMES_IN_FLIGHT );
}

void VulkanComputeExample::init_texture() {
  Logger::log( "Initialing example texture...", Logger::DEBUG );

  // Setup texture
  vulkanTextureImage_ = std::make_shared<Image::VulkanTextureImage>();
  Image::create_texture_image( vulkanDevice_, commandPool_->pool, vulkanTextureImage_,
                               TEXTURE_PATH );
}

void VulkanComputeExample::init_mesh() {
  Logger::log( "Initialing example mesh...", Logger::DEBUG );

  // Setup mesh
  mesh_ = load_mesh( MODEL_PATH );
}

void VulkanComputeExample::init_shader() { SHADER_PATH = "texture_shader.slang.spv"; }

}  // namespace Examples
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy