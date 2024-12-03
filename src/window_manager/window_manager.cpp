
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN

#include "window_manager.hpp"

#include "logger.hpp"
#include "vulkan/vulkan_window.hpp"

extern bool APPLICATION_RUNNING;

namespace Thumpy {
namespace Core {
namespace Windows {

WindowManager::WindowManager( RenderAPI api ) {
  create_new_window( api );
  loop();
}

WindowManager::~WindowManager() { terminate(); }

void WindowManager::create_new_window( RenderAPI api, std::string title ) {
  Window *window;
  switch ( api ) {
    case VULKAN: {
      window = new Vulkan::VulkanWindow( title );
      windows_.push_back( window );
      break;
    }

    case NONE:
    default: {
      window = new Window( title );
      windows_.push_back( window );
    }
  }
}

void WindowManager::loop() {
  // Check if we still have open windows
  if ( windows_.empty() ) {
    // Close application when all windows are closed
    APPLICATION_RUNNING = false;
    return;
  }

  // Loop threw all windows
  for ( int i = windows_.size() - 1; i >= 0; i-- ) {
    // Check for window exit signal
    if ( windows_.at( i )->should_close() ) {
      // Deconstruct window
      windows_.at( i )->deconstruct_window();
      // Delete Window
      windows_.erase( windows_.begin() + i );
      // Resize i
      continue;
    }

    // glfwMakeContextCurrent(windows_.at(i)); ??? maybe

    // run window loop
    windows_.at( i )->loop();

    // // glClear(GL_COLOR_BUFFER_BIT);
    // glfwSwapBuffers(windows_.at(i));
  }  // get next window and repeat

  // Poll input events
  glfwPollEvents();
}

void WindowManager::terminate() {
  Logger::log( "Terminating glfw...", Logger::INFO );
  glfwTerminate();
}

}  // namespace Windows

}  // namespace Core

}  // namespace Thumpy
