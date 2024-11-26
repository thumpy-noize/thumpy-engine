
#pragma once

#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
namespace Thumpy {
namespace Core {
namespace Windows {
class Window {
public:
  Window(std::string title);
  ~Window();
  void init_window();
  void init_vulkan();
  void deconstruct_window();
  void loop();
  bool should_close();

private:
  GLFWwindow *window_;
  std::string title_;
};
} // namespace Windows

} // namespace Core

} // namespace Thumpy
