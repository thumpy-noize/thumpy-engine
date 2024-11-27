/**
 * @file vulkan_framebuffers.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief vulkan_framebuffer cpp file
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vulkan_framebuffers.hpp"
#include "vulkan_initializers.hpp"
#include <stdexcept>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

void create_framebuffers(VulkanSwapChain *swapChain, VkDevice device) {
  swapChain->swapChainFramebuffers.resize(
      swapChain->swapChainImageViews.size());

  for (size_t i = 0; i < swapChain->swapChainImageViews.size(); i++) {
    VkImageView attachments[] = {swapChain->swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo = Initializer::framebuffer_info(
        swapChain->renderPass, swapChain->extent, attachments);

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                            &swapChain->swapChainFramebuffers[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
