
#pragma once

#include "window.hpp"
#include <vector>

namespace Thumpy {
namespace Core {
namespace Windows {

enum RenderAPI { NONE, VULKAN };

class WindowManager {
public:
  WindowManager();
  ~WindowManager();

  // Create new window
  void create_new_window(
      RenderAPI api,
      char *title = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

  // Run loop
  void loop();

  void terminate();

protected:
  std::vector<Window *> windows_;
};
} // namespace Windows

} // namespace Core

} // namespace Thumpy
