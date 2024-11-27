#pragma once

#include <cstdint>
#include <optional>
namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
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
