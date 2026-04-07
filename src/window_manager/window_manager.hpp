
#pragma once

#include <memory>
#include <vector>

#include "window.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {

enum RenderAPI { NONE, VULKAN };

class WindowManager {
 public:
  WindowManager( RenderAPI api );
  ~WindowManager();

  // Create new window
  void create_new_window(
      RenderAPI api, std::string title = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" );

  // Run loop
  void loop();

  void terminate();

 protected:
  std::vector<std::unique_ptr<Window>> windows_;
};
}  // namespace Windows

}  // namespace Core

}  // namespace Thumpy
