#pragma once

#include "window.hpp"
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Thumpy {
namespace Core {
namespace Windows {
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class VulkanWindow : public Window {
public:
  VulkanWindow(std::string title);
  void init_vulkan();

  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();

  bool isDeviceSuitable(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  std::vector<const char *> getRequiredExtensions();

  bool checkValidationLayerSupport();
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  VkInstance instance;
  VkSurfaceKHR surface;

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;

  VkQueue graphicsQueue;
  VkQueue presentQueue;

  VkDebugUtilsMessengerEXT debugMessenger;

#if defined(VK_USE_PLATFORM_XCB_KHR)
  xcb_connection_t *connection;
  xcb_screen_t *screen;
  xcb_window_t window;
  // xcb_intern_atom_reply_t *atom_wm_delete_window;
#endif
};
} // namespace Windows
} // namespace Core
} // namespace Thumpy
