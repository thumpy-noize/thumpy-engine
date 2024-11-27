#pragma once

#include <GLFW/glfw3.h>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool is_complete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class SwapChain {
public:
  SwapChain(VkInstance instance, VkPhysicalDevice physicalDevice,
            VkDevice device, VkSurfaceKHR surface, GLFWwindow *window);
  void create_swap_chain();
  void recreate_swap_chain();
  void clear_swap_chain();

  void set_context(VkInstance instance, VkPhysicalDevice physicalDevice,
                   VkDevice device, VkSurfaceKHR surface, GLFWwindow *window);

  SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);

  VkSurfaceFormatKHR choose_swap_surface_format(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR choose_swap_present_mode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities);
  QueueFamilyIndices find_queue_families(VkPhysicalDevice device);

private:
  VkSwapchainKHR swapChain_;
  VkInstance instance_{VK_NULL_HANDLE};
  VkDevice device_{VK_NULL_HANDLE};
  VkPhysicalDevice physicalDevice_{VK_NULL_HANDLE};
  VkSurfaceKHR surface_{VK_NULL_HANDLE};
  GLFWwindow *window_;
};
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
