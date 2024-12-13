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

#include <vulkan/vulkan_core.h>

#include <vector>

#include "vulkan/vulkan_construct.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_pipeline.hpp"
#include "window.hpp"

class VulkanDevice;
class VulkanRender;

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

class VulkanWindow : public Window {
 public:
  VulkanWindow( std::string title );

#pragma region Core

  /**
   * @brief Setup vulkan
   *
   */
  void init_vulkan();

  /**
   * @brief deconstruct vulkan window
   *
   */
  void deconstruct_window();

  /**
   * @brief Render loop
   *
   */
  void loop();

  // move this to vulkan_construct
  void create_surface();

#pragma endregion Core

  // const std::string TEXTURE_PATH = "vj_swirl.png";

  const std::string MODEL_PATH = "viking_room.obj";
  const std::string TEXTURE_PATH = "viking_room.png";

 private:
  const int MAX_FRAMES_IN_FLIGHT = 2;

  VkInstance instance_;
  VkSurfaceKHR surface_;

  VulkanDevice *vulkanDevice_;
  VulkanSwapChain *swapChain_;
  VulkanPipeline *pipeline_;
  VulkanTextureImage *textureImage_;
  VulkanImage *depthBuffer_;
  VulkanImage *msaaColorBuffer_;
  VulkanRender *render_;

  Construct::CommandPool *commandPool_;
  Mesh *mesh_;

  VkDebugUtilsMessengerEXT debugMessenger_;

  // VkCommandPool commandPool_;
  // std::vector<VkCommandBuffer> commandBuffers_;

  std::vector<VkBuffer> uniformBuffers_;
  std::vector<VkDeviceMemory> uniformBuffersMemory_;
  std::vector<void *> uniformBuffersMapped_;

  VkBuffer vertexBuffer_;
  VkDeviceMemory vertexBufferMemory_;

  VkBuffer indexBuffer_;
  VkDeviceMemory indexBufferMemory_;

  VkDescriptorSetLayout descriptorSetLayout_;
  VkDescriptorPool descriptorPool_;
  std::vector<VkDescriptorSet> descriptorSets_;

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
