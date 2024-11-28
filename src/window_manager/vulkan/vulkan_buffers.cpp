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

#include "vulkan_buffers.hpp"
#include "vulkan_initializers.hpp"
#include <cstring>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Buffer {

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

void create_vertex_buffer(std::vector<Vertex> vertices,
                          VulkanDevice *vulkanDevice, VkBuffer vertexBuffer,
                          VkDeviceMemory vertexBufferMemory) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = sizeof(vertices[0]) * vertices.size();
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(vulkanDevice->device, &bufferInfo, nullptr,
                     &vertexBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create vertex buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(vulkanDevice->device, vertexBuffer,
                                &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      find_memory_type(memRequirements.memoryTypeBits,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       vulkanDevice->physicalDevice);

  if (vkAllocateMemory(vulkanDevice->device, &allocInfo, nullptr,
                       &vertexBufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate vertex buffer memory!");
  }

  vkBindBufferMemory(vulkanDevice->device, vertexBuffer, vertexBufferMemory, 0);

  void *data;
  vkMapMemory(vulkanDevice->device, vertexBufferMemory, 0, bufferInfo.size, 0,
              &data);
  memcpy(data, vertices.data(), (size_t)bufferInfo.size);
  vkUnmapMemory(vulkanDevice->device, vertexBufferMemory);
}

uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                          VkPhysicalDevice physicalDevice) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

} // namespace Buffer
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
