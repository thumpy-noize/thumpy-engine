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

// #include <vulkan/vulkan_core.h>

#include <vulkan/vulkan_raii.hpp>

// #include "vulkan/vulkan_buffers.hpp"
#include "vulkan/vulkan_construct.hpp"
#include "vulkan_helper.hpp"
// #include "vulkan_pipeline.hpp"
// #include "vulkan_render.hpp"
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

  /**
   * @brief deconstruct vulkan window
   *
   */
  virtual void deconstruct_window();

  /**
   * @brief Render loop
   *
   */
  virtual void loop();

  // move this to vulkan_construct
  void create_surface();

#pragma endregion Core

  // const std::string TEXTURE_PATH = "vj_swirl.png";

  // const std::string MODEL_PATH = "viking_room.obj";
  // const std::string TEXTURE_PATH = "viking_room.png";

 protected:
  const int MAX_FRAMES_IN_FLIGHT = 2;

  vk::raii::Context raiiContext_;
  vk::raii::Instance raiiInstance_ = nullptr;
  vk::raii::DebugUtilsMessengerEXT raiiDebugMessenger_ = nullptr;

  // ### None RAII variables ###

  // VkInstance instance_;
  // VkSurfaceKHR surface_;

  std::unique_ptr<VulkanDevice> vulkanDevice_ = nullptr;
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
