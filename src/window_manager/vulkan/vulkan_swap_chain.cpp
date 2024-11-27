#include "vulkan_swap_chain.hpp"
#include <algorithm> // Necessary for std::clamp
#include <limits>
#include <stdexcept>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

SwapChain::SwapChain(VkInstance instance, VkPhysicalDevice physicalDevice,
                     VkDevice device, VkSurfaceKHR surface,
                     GLFWwindow *window) {
  // set_context(instance, physicalDevice, device, surface, window);
  // create_swap_chain();
}

} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
