
#include "logger.hpp"
#include "logger_helper.hpp"
#include "vulkan_window.hpp"
#include "window.hpp"
#include <memory>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "window_manager.hpp"

extern bool APPLICATION_RUNNING;

namespace Thumpy {
namespace Core {
namespace Windows {

WindowManager::WindowManager() {
  create_new_window(VULKAN);
  loop();
}

WindowManager::~WindowManager() { terminate(); }

void WindowManager::create_new_window(RenderAPI api, char *title) {
  Window *window;
  switch (api) {
  case VULKAN: {
    window = new VulkanWindow(title);
    windows_.push_back(window);
    break;
  }

  case NONE: {
    return;
  }

  default: {
    window = new Window(title);
    windows_.push_back(window);
  }
  }
}

void WindowManager::loop() {
  // Check if we still have open windows
  if (windows_.empty()) {
    // Close application when all windows are closed
    APPLICATION_RUNNING = false;
    return;
  }

  // Loop threw all windows
  for (int i = windows_.size() - 1; i >= 0; i--) {
    // Check for window exit signal
    if (windows_.at(i)->should_close()) {
      // Deconstruct window
      windows_.at(i)->deconstruct_window();
      // Delete Window
      windows_.erase(windows_.begin() + i);
      // Resize i
      continue;
    }
    // run window loop
    windows_.at(i)->loop();
  } // get next window and repeat

  // Poll input events
  glfwPollEvents();
}

void WindowManager::terminate() {
  Logger::log("Terminating glfw...", Logger::DEBUG);
  glfwTerminate();
}

} // namespace Windows

} // namespace Core

} // namespace Thumpy
