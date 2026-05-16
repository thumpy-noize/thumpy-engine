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
#include "vulkan_mesh_example.hpp"

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

VulkanMeshExample::VulkanMeshExample( std::string title ) : VulkanWindow( title ) {}

void VulkanMeshExample::init_texture() {
  Logger::log( "Initialing example texture...", Logger::DEBUG );

  // Setup texture
  vulkanTextureImage_ = std::make_shared<Image::VulkanTextureImage>();
  Image::create_texture_image( vulkanDevice_, commandPool_->pool, vulkanTextureImage_,
                               TEXTURE_PATH );
}

void VulkanMeshExample::init_mesh() {
  Logger::log( "Initialing example mesh...", Logger::DEBUG );

  // Setup mesh
  mesh_ = load_mesh( MODEL_PATH );
}

void VulkanMeshExample::init_shader() { SHADER_PATH = "texture_shader.slang.spv"; }

}  // namespace Examples
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy