#pragma once

#include <cstdint>
#include <optional>
#include <vector>
namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

struct QueueFamilyIndices {

  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool is_complete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
