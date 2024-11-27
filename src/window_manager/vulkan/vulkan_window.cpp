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
#include "vulkan/vulkan_device.hpp"
#define GLFW_INCLUDE_VULKAN

#include "logger.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan_command.hpp"
#include "vulkan_framebuffers.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_initializers.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_window.hpp"
#include <cstdint> // Necessary for uint32_t
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

// Create simple io script to hold this
static std::vector<char> read_file(const std::string &filename) {
  Logger::log("opening file: " + filename, Logger::INFO);
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file: " + filename);
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

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
  create_framebuffers(swapChain_, vulkanDevice_->device);

  // Create command pool
  create_command_pool(vulkanDevice_, commandPool_);

  create_command_buffer();
  create_sync_objects();
}

void VulkanWindow::deconstruct_window() {
  Logger::log("Destroying vulkan...");

  swapChain_->clear_swap_chain();

  destroy_graphics_pipeline(vulkanDevice_->device, pipeline_);
  // vkDestroyPipeline(vulkanDevice_->device, graphicsPipeline_, nullptr);
  // vkDestroyPipelineLayout(vulkanDevice_->device, pipelineLayout_, nullptr);

  vkDestroyRenderPass(vulkanDevice_->device, swapChain_->renderPass, nullptr);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(vulkanDevice_->device, renderFinishedSemaphores_[i],
                       nullptr);
    vkDestroySemaphore(vulkanDevice_->device, imageAvailableSemaphores_[i],
                       nullptr);
    vkDestroyFence(vulkanDevice_->device, inFlightFences_[i], nullptr);
  }

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

void VulkanWindow::loop() {
  Window::loop();
  draw_frame();

  vkDeviceWaitIdle(vulkanDevice_->device);
}

#pragma endregion Core

#pragma region Image

void VulkanWindow::create_command_buffer() {
  commandBuffers_.resize(MAX_FRAMES_IN_FLIGHT);
  VkCommandBufferAllocateInfo allocInfo =
      Initializer::command_buffer_allocate_info(
          commandPool_, (uint32_t)commandBuffers_.size());

  if (vkAllocateCommandBuffers(vulkanDevice_->device, &allocInfo,
                               commandBuffers_.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void VulkanWindow::record_command_buffer(VkCommandBuffer commandBuffer,
                                         uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo = Initializer::command_buffer_begin_info();

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo = Initializer::render_pass_info(
      swapChain_->renderPass, swapChain_->swapChainFramebuffers[imageIndex],
      swapChain_->extent);

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; // background color
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline_.graphicsPipeline);

  VkViewport viewport =
      Initializer::viewport(static_cast<float>(swapChain_->extent.height),
                            static_cast<float>(swapChain_->extent.width));
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor = Initializer::scissor(swapChain_->extent);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  vkCmdDraw(commandBuffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

void VulkanWindow::draw_frame() {
  vkWaitForFences(vulkanDevice_->device, 1, &inFlightFences_[currentFrame_],
                  VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      vulkanDevice_->device, swapChain_->swapChain, UINT64_MAX,
      imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    swapChain_->recreate_swap_chain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  vkResetFences(vulkanDevice_->device, 1, &inFlightFences_[currentFrame_]);

  vkResetCommandBuffer(commandBuffers_[currentFrame_],
                       /*VkCommandBufferResetFlagBits*/ 0);
  record_command_buffer(commandBuffers_[currentFrame_], imageIndex);

  // vkAcquireNextImageKHR(vulkanDevice_->device, swapChain_, UINT64_MAX,
  //                       imageAvailableSemaphores_[currentFrame_],
  //                       VK_NULL_HANDLE, &imageIndex);

  // submit command buffer
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores_[currentFrame_]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphores_[currentFrame_]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(vulkanDevice_->graphicsQueue, 1, &submitInfo,
                    inFlightFences_[currentFrame_]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swapChain_->swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr; // Optional

  result = vkQueuePresentKHR(vulkanDevice_->presentQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      framebufferResized) {
    framebufferResized = false;
    swapChain_->recreate_swap_chain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanWindow::create_sync_objects() {
  imageAvailableSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo = Initializer::semaphore_info();

  VkFenceCreateInfo fenceInfo = Initializer::fence_info();

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(vulkanDevice_->device, &semaphoreInfo, nullptr,
                          &imageAvailableSemaphores_[i]) != VK_SUCCESS ||
        vkCreateSemaphore(vulkanDevice_->device, &semaphoreInfo, nullptr,
                          &renderFinishedSemaphores_[i]) != VK_SUCCESS ||
        vkCreateFence(vulkanDevice_->device, &fenceInfo, nullptr,
                      &inFlightFences_[i]) != VK_SUCCESS) {

      throw std::runtime_error(
          "failed to create synchronization objects for a frame!");
    }
  }
}

#pragma endregion Image

} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
