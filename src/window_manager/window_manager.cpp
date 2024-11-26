
#include "logger.hpp"
#include "logger_helper.hpp"
#include "window.hpp"
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
  create_new_window();
  loop();
}

WindowManager::~WindowManager() { terminate(); }

void WindowManager::create_new_window(char *title) {
  Window *window = new Window(title);
  windows_.push_back(window);
}

void WindowManager::loop() {
  // Check if we still have open windows
  while (!windows_.empty()) {
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

    // check for exit signal
    if (!APPLICATION_RUNNING) {
      return;
    }
  } // Loop again until windows are closed

  // Kill window manager
}

void WindowManager::terminate() {
  Logger::log("Terminating glfw...", Logger::DEBUG);
  glfwTerminate();
}

} // namespace Windows

} // namespace Core

} // namespace Thumpy
