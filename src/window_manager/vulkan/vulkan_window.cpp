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
#include "logger.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan_buffers.hpp"
#include "vulkan_command.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_initializers.hpp"
#include "vulkan_pipeline.hpp"
#include <cstdint> // Necessary for uint32_t
#include <cstring>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

#pragma region Core

VulkanWindow::VulkanWindow(std::string title) : Window(title) { init_vulkan(); }

void VulkanWindow::init_vulkan() {

  // Create our instance
  create_instance();
  // Setup debug messenger
  Debug::setup_debug_messenger(instance_, &debugMessenger_);
  // Create surface
  create_surface();
  // Find & create vulkan device
  vulkanDevice_ = new VulkanDevice(instance_, surface_);
  // Create swap chain / image views / render pass
  swapChain_ = new VulkanSwapChain(instance_, vulkanDevice_, surface_, window_);

  // Create graphics pipeline
  pipeline_ = create_graphics_pipeline(swapChain_, vulkanDevice_->device);

  // Create frame buffers
  Buffer::create_framebuffers(swapChain_, vulkanDevice_->device);

  // Create command pool & buffer
  create_command_pool(vulkanDevice_, commandPool_);

  // Create vertex buffer
  vertexBuffer_ = VkBuffer();
  create_vertex_buffer();

  // Create command buffer

  create_command_buffer(commandBuffers_, commandPool_, vulkanDevice_->device,
                        MAX_FRAMES_IN_FLIGHT);

  // Create render
  render_ = new VulkanRender(MAX_FRAMES_IN_FLIGHT, vulkanDevice_, swapChain_,
                             &commandBuffers_, pipeline_);
}

void VulkanWindow::deconstruct_window() {
  Logger::log("Destroying vulkan...");

  swapChain_->clear_swap_chain();

  destroy_graphics_pipeline(vulkanDevice_->device, pipeline_);
  // vkDestroyPipeline(vulkanDevice_->device, graphicsPipeline_, nullptr);
  // vkDestroyPipelineLayout(vulkanDevice_->device, pipelineLayout_, nullptr);

  vkDestroyBuffer(vulkanDevice_->device, vertexBuffer_, nullptr);
  vkFreeMemory(vulkanDevice_->device, vertexBufferMemory_, nullptr);

  vkDestroyRenderPass(vulkanDevice_->device, swapChain_->renderPass, nullptr);

  render_->destroy();

  vkDestroyCommandPool(vulkanDevice_->device, commandPool_, nullptr);

  vkDestroyDevice(vulkanDevice_->device, nullptr);

  if (enableValidationLayers) {
    Debug::destroy_debug_utils_messenger_ext(instance_, &debugMessenger_,
                                             nullptr);
  }

  vkDestroySurfaceKHR(instance_, surface_, nullptr);
  vkDestroyInstance(instance_, nullptr);

  Window::deconstruct_window();
}

void VulkanWindow::loop() {
  Window::loop();
  render_->draw_frame(vertexBuffer_, static_cast<uint32_t>(vertices_.size()));

  vkDeviceWaitIdle(vulkanDevice_->device);
}

void VulkanWindow::create_instance() {

  if (enableValidationLayers && !check_validation_layer_support()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  VkApplicationInfo appInfo = Initializer::application_info();

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto extensions = get_required_extensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    Debug::populate_debug_messenger_create_info(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;

    createInfo.pNext = nullptr;
  }

  if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}

void VulkanWindow::create_surface() {
  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void VulkanWindow::create_vertex_buffer() {

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = sizeof(vertices_[0]) * vertices_.size();
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(vulkanDevice_->device, &bufferInfo, nullptr,
                     &vertexBuffer_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create vertex buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(vulkanDevice_->device, vertexBuffer_,
                                &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = find_memory_type(
      memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  if (vkAllocateMemory(vulkanDevice_->device, &allocInfo, nullptr,
                       &vertexBufferMemory_) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate vertex buffer memory!");
  }

  vkBindBufferMemory(vulkanDevice_->device, vertexBuffer_, vertexBufferMemory_,
                     0);

  void *data;
  vkMapMemory(vulkanDevice_->device, vertexBufferMemory_, 0, bufferInfo.size, 0,
              &data);
  memcpy(data, vertices_.data(), (size_t)bufferInfo.size);
  vkUnmapMemory(vulkanDevice_->device, vertexBufferMemory_);
}

uint32_t VulkanWindow::find_memory_type(uint32_t typeFilter,
                                        VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(vulkanDevice_->physicalDevice,
                                      &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

#pragma endregion Core

} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
