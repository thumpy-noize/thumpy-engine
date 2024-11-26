#pragma once

#include "window.hpp"
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Thumpy {
namespace Core {
namespace Windows {

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class VulkanWindow : public Window {
public:
  VulkanWindow(std::string title);
  void init_vulkan();
  void deconstruct_window();

  void create_instance();
  void setup_debug_messenger();
  void create_surface();

#pragma region Devices

  void pick_physical_device();
  void create_logical_device();
  /// Checks if device is usable
  bool is_device_suitable(VkPhysicalDevice device);

#pragma region SwapChain
  void create_swap_chain();
  VkSurfaceFormatKHR choose_swap_surface_format(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR choose_swap_present_mode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities);
  /// Finds swapchain details
  SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);
#pragma endregion SwapChain

  bool check_device_extension_support(VkPhysicalDevice device);
  QueueFamilyIndices find_queue_families(VkPhysicalDevice device);

#pragma endregion Devices

  std::vector<const char *> get_required_extensions();

  bool check_validation_layer_support();
  void populate_debug_messenger_create_info(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

private:
  VkInstance instance_;
  VkSurfaceKHR surface_;

  VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
  VkDevice device_;

  VkSwapchainKHR swapChain_;
  std::vector<VkImage> swapChainImages_;
  VkFormat swapChainImageFormat_;
  VkExtent2D swapChainExtent_;

  VkQueue graphicsQueue_;
  VkQueue presentQueue_;

  VkDebugUtilsMessengerEXT debugMessenger_;
};
} // namespace Windows
} // namespace Core
} // namespace Thumpy
