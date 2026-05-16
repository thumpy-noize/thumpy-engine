/**
 * @file vulkan_window.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Vulkan support for our glfw window.
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#define GLFW_INCLUDE_VULKAN

// #include <vulkan/vulkan_core.h>
// #include "vulkan/vulkan_buffers.hpp"

#include <vulkan/vulkan_raii.hpp>

#include "vulkan_construct.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_image.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_render.hpp"
#include "vulkan_swap_chain.hpp"
#include "window.hpp"

class VulkanDevice;

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

class VulkanWindow : public Window {
 public:
  VulkanWindow( std::string title );
  ~VulkanWindow() override { deconstruct_window(); };

#pragma region Core

  /**
   * @brief Setup vulkan
   *
   */
  virtual void init_vulkan();

  virtual void init_texture();

  virtual void init_mesh();

  virtual void init_shader();

  /**
   * @brief Deconstruct vulkan window
   *
   */
  virtual void deconstruct_window();

  /**
   * @brief Render loop
   *
   */
  virtual void loop();

  /**
   * @brief Create surface
   *
   */
  void create_surface();

#pragma endregion Core

  // const std::string TEXTURE_PATH = "vj_swirl.png";

  const std::string MODEL_PATH = "viking_room.obj";
  const std::string TEXTURE_PATH = "viking_room.png";
  std::string SHADER_PATH = "texture_shader.slang.spv";

 protected:
  const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

  vk::raii::Instance instance_ = nullptr;                      // Instance
  vk::raii::Context context_;                                  // Context
  vk::raii::DebugUtilsMessengerEXT debugMessenger_ = nullptr;  // Debug messenger
  std::shared_ptr<vk::raii::SurfaceKHR> surface_ = nullptr;    // Surface

  std::shared_ptr<VulkanDevice> vulkanDevice_ = nullptr;  // Vulkan device
  std::shared_ptr<VulkanSwapChain> swapChain_ = nullptr;  // Swap chain
  std::shared_ptr<VulkanPipeline> pipeline_ = nullptr;    // Pipeline
  std::shared_ptr<VulkanRender> render_ = nullptr;        // Render

  std::shared_ptr<Construct::CommandPool> commandPool_ = nullptr;     // Command pool
  std::shared_ptr<Buffer::Buffer> vertexBuffer_ = nullptr;            // Vertex buffer
  std::shared_ptr<Buffer::Buffer> indexBuffer_ = nullptr;             // Index buffer
  std::shared_ptr<Buffer::UniformBuffers> uniformBuffers_ = nullptr;  // Uniform buffers
  std::shared_ptr<Descriptors> descriptors_ = nullptr;                // Descriptors

  std::shared_ptr<Image::VulkanTextureImage> vulkanTextureImage_ = nullptr;  // Vulkan texture Image
  std::shared_ptr<Image::VulkanImage> depthBuffer_ = nullptr;                // Depth buffer
  std::shared_ptr<Image::VulkanImage> colorImage_ = nullptr;                 // Msaa color image

  std::shared_ptr<Mesh> mesh_ = nullptr;  // Mesh object

  // TODO: This is for triangle testing. This should be moved to a child class,
  // like vulkan_triangle_example
  // const std::vector<Vertex> vertices_ = { { { 0.2f, -0.9f }, { 1.0f, 0.0f, 0.2f } },
  //                                         { { 0.9f, 0.9f }, { 0.2f, 1.0f, 0.0f } },
  //                                         { { -0.9f, 0.5f }, { 0.0f, 0.2f, 1.0f } } };

  // Square vertices, move this also
  // const std::vector<Vertex> vertices_ = { { { -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f } },
  //                                         { { 1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } },
  //                                         { { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
  //                                         { { -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } } };

  // Square texture vertices, move this also
  // const std::vector<Vertex> vertices_ = {
  //     { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
  //     { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
  //     { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
  //     { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } } };

  // 2 Squares with textures
  // const std::vector<Vertex> vertices_ = {
  //     { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
  //     { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
  //     { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
  //     { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },

  //     { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
  //     { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
  //     { { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
  //     { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } } };

  // Square indices, move this also
  // const std::vector<uint16_t> indices_ = { 0, 1, 2, 2, 3, 0 };

  // 2 Square indices
  // const std::vector<uint16_t> indices_ = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };

  // ### None RAII variables (Deprecated) ###

  // VkInstance instance_;
  // VkSurfaceKHR surface_;

  // VulkanSwapChain *swapChain_ = nullptr;
  // VulkanPipeline *pipeline_ = nullptr;
  // VulkanTextureImage *textureImage_ = nullptr;
  // VulkanImage *depthBuffer_ = nullptr;
  // VulkanImage *msaaColorBuffer_ = nullptr;
  // VulkanRender *render_ = nullptr;

  // Construct::CommandPool *commandPool_ = nullptr;
  // Construct::UniformBuffers *uniformBuffers_ = nullptr;
  // Buffer::Buffer *vertexBuffer_ = nullptr;
  // Buffer::Buffer *indexBuffer_ = nullptr;
  // Descriptors *descriptors_ = nullptr;

  // Mesh *mesh_ = nullptr;

  // VkDebugUtilsMessengerEXT debugMessenger_;

  // warp t
  // std::vector<Vertex> vertices_ = {
  //     {{0.0f, -1.0f}, {1.0f, 0.0f, 1.0f}}, {{1.0f, 1.0f}, {0.0f, 1.0f,
  //     0.0f}},
  //     {{-1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, {{0.0f, -0.9f}, {1.0f,
  //     0.0f, 1.0f}},
  //     {{0.9f, 0.9f}, {0.0f, 1.0f, 0.0f}},  {{-0.9f, 0.9f}, {0.0f,
  //     0.0f, 1.0f}},
  //     {{0.0f, -0.8f}, {1.0f, 0.0f, 1.0f}}, {{0.8f, 0.8f}, {0.0f, 1.0f,
  //     0.0f}},
  //     {{-0.8f, 0.8f}, {0.0f, 0.0f, 1.0f}}, {{0.0f, -0.7f}, {1.0f,
  //     0.0f, 1.0f}},
  //     {{0.7f, 0.7f}, {0.0f, 1.0f, 0.0f}},  {{-0.7f, 0.7f}, {0.0f,
  //     0.0f, 1.0f}},
  //     {{0.0f, -0.6f}, {1.0f, 0.0f, 1.0f}}, {{0.6f, 0.6f}, {0.0f, 1.0f,
  //     0.0f}},
  //     {{-0.6f, 0.6f}, {0.0f, 0.0f, 1.0f}}, {{0.0f, -0.5f}, {1.0f,
  //     0.0f, 1.0f}},
  //     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},  {{-0.5f, 0.5f}, {0.0f,
  //     0.0f, 1.0f}},
  //     {{0.0f, -0.4f}, {1.0f, 0.0f, 1.0f}}, {{0.4f, 0.4f}, {0.0f, 1.0f,
  //     0.0f}},
  //     {{-0.4f, 0.4f}, {0.0f, 0.0f, 1.0f}}, {{0.0f, -0.3f}, {1.0f,
  //     0.0f, 1.0f}},
  //     {{0.3f, 0.3f}, {0.0f, 1.0f, 0.0f}},  {{-0.3f, 0.3f}, {0.0f,
  //     0.0f, 1.0f}},
  //     {{0.0f, -0.2f}, {1.0f, 0.0f, 1.0f}}, {{0.2f, 0.2f}, {0.0f, 1.0f,
  //     0.0f}},
  //     {{-0.2f, 0.2f}, {0.0f, 0.0f, 1.0f}}, {{0.0f, -0.1f}, {1.0f,
  //     0.0f, 1.0f}},
  //     {{0.1f, 0.1f}, {0.0f, 1.0f, 0.0f}},  {{-0.1f, 0.1f}, {0.0f,
  //     0.0f, 1.0f}}};

  // fill screen
  // std::vector<Vertex> vertices_ = { { { 0.0f, -1.0f }, { 1.0f, 0.0f, 1.0f
  // }
  // },
  //                                   { { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f
  //                                   } }, { { -1.0f, 1.0f }, { 0.0f,
  //                                   0.0f, 1.0f }
  //                                   }, { { 0.0f, 1.0f }, { 1.0f,
  //                                   0.0f, 1.0f }
  //                                   }, { { -1.0f, -1.0f }, { 0.0f, 1.0f,
  //                                   0.0f } }, { { 1.0f, -1.0f }, { 0.0f,
  //                                   0.0f, 1.0f } }, { { -1.0f, 1.0f },
  //                                   { 1.0f, 0.0f, 1.0f } }, { { -1.0f,
  //                                   -1.0f
  //                                   }, { 0.0f, 1.0f, 0.0f } }, { { -0.5f,
  //                                   0.0f }, { 0.0f, 0.0f, 1.0f } }, {
  //                                   { 1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }
  //                                   }, { { 1.0f, 1.0f }, { 0.0f, 1.0f,
  //                                   0.0f } }, { { 0.5f, 0.0f }, { 0.0f,
  //                                   0.0f, 1.0f } }
  //                                   };

  // square
  // const std::vector<Vertex> vertices_ = {
  //     { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
  //     { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
  //     { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
  //     { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } } };

  // const std::vector<uint16_t> indices_ = { 0, 1, 2, 2, 3, 0 };

  // const std::vector<Vertex> vertices_ = {
  //     { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
  //     { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
  //     { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
  //     { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },

  //     { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
  //     { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
  //     { { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
  //     { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } } };

  // const std::vector<uint16_t> indices_ = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };
};
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
