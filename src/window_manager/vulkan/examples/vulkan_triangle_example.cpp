/**
 * @file vulkan_triangle.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Creates a window with a sierpinski's triangle
 * TODO:
 * Modify this script to create a static sierpinski's triangle. Others scripts will need to be
 * modified as well to acomiddate multiple examples.
 * @version 0.1
 * @date 2024-12-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "vulkan_triangle_example.hpp"

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

void VulkanTriangleExample::init_vulkan() {
  // Find & create vulkan device
  vulkanDevice_ = new VulkanDevice( instance_, surface_ );

  // Create swap chain / image views / render pass
  swapChain_ = new VulkanSwapChain( vulkanDevice_, window_, surface_ );

  // Todo: Update this
  // Create descriptor layouts
  descriptors_ = new Descriptors();
  Construct::descriptor_set_layout( vulkanDevice_, descriptors_->setLayout );

  // Todo: Update this
  // Create graphics pipeline
  pipeline_ = create_graphics_pipeline( swapChain_, vulkanDevice_, descriptors_->setLayout );

  // Multisampling
  msaaColorBuffer_ = new VulkanImage();
  Image::create_color_resources( msaaColorBuffer_, vulkanDevice_, swapChain_ );

  // Depth buffer / Not required for this example
  depthBuffer_ = new VulkanImage();
  Image::create_depth_resources( depthBuffer_, vulkanDevice_, swapChain_->extent );

  // Create frame buffers
  Buffer::create_framebuffers( swapChain_, depthBuffer_->imageView, msaaColorBuffer_->imageView,
                               vulkanDevice_->device );

  // Create command pool
  commandPool_ = new Construct::CommandPool();
  Construct::command_pool( vulkanDevice_, commandPool_->pool );

  // Todo: Remove this
  // Create texture image / view / sampler
  textureImage_ = new VulkanTextureImage();
  Image::create_texture_image( vulkanDevice_, textureImage_, commandPool_->pool, TEXTURE_PATH );
  Image::create_texture_image_view( vulkanDevice_->device, textureImage_ );
  Image::create_texture_sampler( vulkanDevice_, textureImage_ );

  // Create vertex buffer

  mesh_ = Shapes::generate_triangle();
  // mesh_ = Shapes::generate_square();
  // mesh_ = load_mesh( MODEL_PATH );

  mesh_ = Shapes::generate_sierpinski_triangle( mesh_, 6 );

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

  // Todo: Update this
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

void VulkanTriangleExample::deconstruct_window() {
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

  Window::deconstruct_window();
}

}  // namespace Examples
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy